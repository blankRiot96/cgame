#include "networking.h"
#include "shared.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void serialize_packet(Packet *p, char *buffer, int *offset) {
    // Serialize the current client's packet (square_x, square_y)
    int square_x_network = htonl(p->square_x);
    int square_y_network = htonl(p->square_y);
    memcpy(buffer + *offset, &square_x_network, sizeof(int));
    *offset += sizeof(int);
    memcpy(buffer + *offset, &square_y_network, sizeof(int));
    *offset += sizeof(int);
}

void deserialize_packet(char *buffer, Packet *p, int *offset) {
    // Deserialize the packet (square_x, square_y)
    p->square_x = ntohl(*(int *)(buffer + *offset));
    *offset += sizeof(int);
    p->square_y = ntohl(*(int *)(buffer + *offset));
    *offset += sizeof(int);
}

void ipv4_to_base64(const char *ipv4, char *output) {
    uint32_t ip_as_int = 0;
    int segments[4];

    // Parse the IPv4 address into its 4 segments
    if (sscanf(ipv4, "%d.%d.%d.%d", &segments[0], &segments[1], &segments[2],
               &segments[3]) != 4) {
        fprintf(stderr, "Invalid IPv4 address format.\n");
        strcpy(output, "ERROR"); // Ensure proper error handling
        return;
    }

    // Combine the 4 segments into a 32-bit integer (big-endian order)
    ip_as_int = (segments[0] << 24) | (segments[1] << 16) | (segments[2] << 8) |
                segments[3];

    // Convert the 32-bit integer to base64 (using 6 characters)
    for (int i = 0; i < 6; i++) {
        int index = (ip_as_int >> (30 - i * 6)) &
                    0x3F; // Shift and mask to get the base64 index
        output[i] = BASE64_ALPHABET[index];
    }

    // Null-terminate the string
    output[6] = '\0'; // Ensure null termination
}

void base64_to_ipv4(const char *base64, char *output) {
    uint32_t ip_as_int = 0;

    // Allow 5 or 6 characters for Base64 input
    size_t length = strlen(base64);
    if (length != 5 && length != 6) {
        fprintf(stderr, "Invalid Base64 code length: %zu\n", length);
        strcpy(output, "ERROR");
        return;
    }

    for (size_t i = 0; i < length; i++) {
        const char *ptr = strchr(BASE64_ALPHABET, base64[i]);
        if (!ptr) {
            fprintf(stderr, "Invalid character in Base64 code: %c\n",
                    base64[i]);
            strcpy(output, "ERROR");
            return;
        }
        ip_as_int = (ip_as_int << 6) | (ptr - BASE64_ALPHABET);
    }

    // Adjust for shorter inputs (5 characters = 30 bits)
    if (length == 5) {
        ip_as_int <<= 2; // Add 2 padding bits
    }

    snprintf(output, 16, "%d.%d.%d.%d", (ip_as_int >> 24) & 0xFF,
             (ip_as_int >> 16) & 0xFF, (ip_as_int >> 8) & 0xFF,
             ip_as_int & 0xFF);
}

int client_sockets[MAX_CLIENTS];
int num_clients = 0;

// Function to broadcast a message to all clients except the sender
void broadcast_message(int sender_socket, const char *message) {
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] != sender_socket) {
            if (send(client_sockets[i], message, strlen(message), 0) == -1) {
                perror("Failed to send message to client");
            }
        }
    }
}

int get_client_index(int client_socket) {
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] == client_socket) {
            return i;
        }
    }
    return -1; // Client not found
}

void store_client_name(int client_socket, const char *name) {
    int client_index = get_client_index(client_socket);
    if (client_index != -1) {
        strncpy(shared.other_client_names[client_index], name,
                sizeof(shared.other_client_names[client_index]) - 1);
    }
}

void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Receive the client's name first
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        // Store the client name
        store_client_name(client_socket, buffer);
    }

    // After storing the name, send the full list of clients to everyone
    broadcast_client_state(client_socket);

    while ((bytes_received =
                recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';

        // Deserialize received data into the client's packet
        int offset = 0;
        deserialize_packet(buffer, &shared.client_packet, &offset);

        // Store the received packet in shared.other_client_packets
        // Assuming the message contains the packets of other clients
        int client_index = get_client_index(client_socket);
        if (client_index != -1) {
            // Assume that each message contains one packet from each client
            for (int i = 0; i < num_clients; i++) {
                if (i != client_index) {
                    deserialize_packet(buffer, &shared.other_client_packets[i],
                                       &offset);
                }
            }
        }

        // Broadcast the message to other clients
        broadcast_message(client_socket, buffer);
    }
    // Remove client and close socket logic as before
}

void *start_server(void *arg) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Server started, waiting for connections...\n");

    while (1) {
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                               &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        // Add the new client socket to the list
        if (num_clients < MAX_CLIENTS) {
            client_sockets[num_clients++] = client_socket;
            printf("Client connected, total clients: %d\n");
        } else {
            printf("Maximum client limit reached.\n");
            close(client_socket);
            continue;
        }

        // Allocate memory for the client socket and pass it to the thread
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        // Create a new thread for each client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client,
                           client_socket_ptr) != 0) {
            perror("Thread creation failed");
            close(client_socket);
            free(client_socket_ptr);
        }

        // Detach the thread so it can clean up resources when done
        pthread_detach(thread_id);
    }

    close(server_socket);

    return NULL;
}

void broadcast_client_state(int sender_socket) {
    char buffer[BUFFER_SIZE];
    int offset = 0; // Initialize offset

    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] != sender_socket) {
            // Serialize the client's packet and name, and send it
            serialize_packet(&shared.client_packet, buffer,
                             &offset); // Pass offset
            // Send the packet and name
            send(client_sockets[i], buffer, offset,
                 0); // Use offset for correct size
        }
    }
}

void *receive_messages(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            // Deserialize the received packet into the respective client's data
            int offset = 0; // Initialize offset
            deserialize_packet(buffer, &shared.client_packet,
                               &offset); // Pass offset

            // Update other client states
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] != client_socket) {
                    shared.other_client_packets[i] = shared.client_packet;
                }
            }
        }
    }
}

void *start_client(void *arg) {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    printf("Client started...\n");
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    shared.client_socket = client_socket;

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    printf("SERVER IP: %s\n", shared.server_ipv4);
    if (inet_pton(AF_INET, shared.server_ipv4, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(1);
    }

    printf("Moshi moshi?\n");
    printf("[%s] connected to server\n", shared.client_name);

    // Send the client name to the server
    send(client_socket, shared.client_name, strlen(shared.client_name), 0);

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages,
                       (void *)&client_socket) != 0) {
        perror("Thread creation failed");
        close(client_socket);
        exit(1);
    }

    while (1) {
        // Serialize the current client's packet
        int offset = 0;
        serialize_packet(&shared.client_packet, message, &offset);

        // Send the current client's packet along with others
        send(client_socket, message, offset, 0);
    }

    close(client_socket);
    return NULL;
}

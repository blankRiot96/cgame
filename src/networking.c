#include "networking.h"
#include "shared.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void serialize_packet(Packet *p, char *buffer) {
    // Convert each field to network byte order if needed
    int square_x_network = htonl(p->square_x);
    int square_y_network = htonl(p->square_y);

    // Copy each field into the buffer
    memcpy(buffer, &square_x_network, sizeof(int));
    memcpy(buffer + sizeof(int), &square_y_network, sizeof(int));
}

void deserialize_packet(char *buffer, Packet *p) {
    // Copy the data from the buffer and convert from network byte order
    p->square_x = ntohl(*(int *)buffer);
    p->square_y = ntohl(*(int *)(buffer + sizeof(int)));
}

void ipv4_to_base64(const char *ipv4, char *output) {
    uint32_t ip_as_int = 0;
    int segments[4];

    if (sscanf(ipv4, "%d.%d.%d.%d", &segments[0], &segments[1], &segments[2],
               &segments[3]) != 4) {
        fprintf(stderr, "Invalid IPv4 address format.\n");
        strcpy(output, "ERROR");
        return;
    }

    ip_as_int = (segments[0] << 24) | (segments[1] << 16) | (segments[2] << 8) |
                segments[3];

    for (int i = 0; i < 6; i++) {
        int index = (ip_as_int >> (30 - i * 6)) & 0x3F;
        output[i] = BASE64_ALPHABET[index];
    }
    output[6] = '\0';
}

void base64_to_ipv4(const char *base64, char *output) {
    uint32_t ip_as_int = 0;

    if (strlen(base64) != 6) {
        fprintf(stderr, "Invalid Base64 code length.\n");
        strcpy(output, "ERROR");
        return;
    }

    for (int i = 0; i < 6; i++) {
        const char *ptr = strchr(BASE64_ALPHABET, base64[i]);
        if (!ptr) {
            fprintf(stderr, "Invalid character in Base64 code.\n");
            strcpy(output, "ERROR");
            return;
        }
        ip_as_int = (ip_as_int << 6) | (ptr - BASE64_ALPHABET);
    }

    snprintf(output, 16, "%d.%d.%d.%d", (ip_as_int >> 24) & 0xFF,
             (ip_as_int >> 16) & 0xFF, (ip_as_int >> 8) & 0xFF,
             ip_as_int & 0xFF);
}

void *start_server(void *arg) {
    printf("Server started...\n");
    return NULL;
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

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
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

    printf("Connected to server\n");

    while (1) {
        serialize_packet(&shared.client_packet, message);

        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("Send failed");
            break;
        };

        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Server: %s\n", buffer);
        } else {
            printf("Server disconnected\n");
            break;
        }
    }

    return NULL;
}

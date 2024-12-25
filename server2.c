#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// List to store client sockets
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

// Function to handle client communication
void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received =
                recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("Received from client: %s\n", buffer);

        // Broadcast the message to all clients
        broadcast_message(client_socket, buffer);
    }

    if (bytes_received == 0) {
        printf("Client disconnected\n");
    } else {
        perror("recv failed");
    }

    // Remove the client from the list and close the socket
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] == client_socket) {
            for (int j = i; j < num_clients - 1; j++) {
                client_sockets[j] = client_sockets[j + 1];
            }
            num_clients--;
            break;
        }
    }

    close(client_socket);
    free(client_socket_ptr);
    return NULL;
}

int main() {
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
    return 0;
}

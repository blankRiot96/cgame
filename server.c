#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Function to handle client communication
void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received =
                recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("Received: %s\n", buffer);
        send(client_socket, "Message received", 16, 0); // Send response
    }

    if (bytes_received == 0) {
        printf("Client disconnected\n");
    } else {
        perror("recv failed");
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

        printf("Client connected\n");

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

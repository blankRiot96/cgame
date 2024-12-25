#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to receive messages from the server and print them
void *receive_messages(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Message from server: %s\n", buffer);
        }
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(1);
    }

    printf("Connected to server\n");

    // Receive messages from server in a separate thread
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages,
                       (void *)&client_socket) != 0) {
        perror("Thread creation failed");
        close(client_socket);
        exit(1);
    }

    // Send messages to server
    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);
        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
    return 0;
}

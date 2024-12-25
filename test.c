#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int square_x;
    int square_y;
} Packet;

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

int main() {
    Packet packet = {10, 20};
    char buffer[2 * sizeof(int)]; // Buffer to hold the serialized data

    serialize_packet(&packet, buffer);

    // Print the serialized buffer (for debugging)
    for (int i = 0; i < sizeof(buffer); ++i) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");

    Packet deserizalized_packet;
    deserialize_packet(buffer, &deserizalized_packet);

    printf("square_x: %d, square_y: %d\n", packet.square_x, packet.square_y);

    return 0;
}

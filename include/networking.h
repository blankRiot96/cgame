#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BASE64_ALPHABET                                                        \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int square_x;
    int square_y;
} Packet;

void ipv4_to_base64(const char *ipv4, char *output);
void base64_to_ipv4(const char *base64, char *output);

void *start_server(void *arg);
void *start_client(void *arg);

void serialize_packet(Packet *p, char *buffer, int *offset);
void deserialize_packet(char *buffer, Packet *p, int *offset);

void broadcast_client_state(int sender_socket);

#endif

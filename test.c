#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BASE64_ALPHABET                                                        \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

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

int main() { return 0; }

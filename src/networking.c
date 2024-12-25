#include "networking.h"

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

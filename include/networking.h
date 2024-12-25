#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BASE64_ALPHABET                                                        \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

void ipv4_to_base64(const char *ipv4, char *output);
void base64_to_ipv4(const char *base64, char *output);

#endif
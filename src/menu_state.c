#include "menu_state.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include "shared.h"
#include <stdio.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_CODE_LENGTH 6

const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 100;

char *get_internet_ip() {
    struct ifaddrs *ifaddr, *ifa;
    char *ip_address = NULL;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_INET &&
            strcmp(ifa->ifa_name, "lo") != 0) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            ip_address = malloc(INET_ADDRSTRLEN);
            if (ip_address == NULL) {
                perror("malloc");
                freeifaddrs(ifaddr);
                return NULL;
            }
            inet_ntop(AF_INET, &sa->sin_addr, ip_address, INET_ADDRSTRLEN);
            break;
        }
    }

    freeifaddrs(ifaddr);
    return ip_address;
}

void int_to_base36(int num, char *output, int outputSize) {
    const char *base36 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char buffer[outputSize];
    int index = 0;

    do {
        buffer[index++] = base36[num % 36];
        num /= 36;
    } while (num > 0 && index < outputSize - 1);

    buffer[index] = '\0';

    for (int i = 0; i < index; i++) {
        output[i] = buffer[index - i - 1];
    }
    output[index] = '\0';
}

void ipv4_to_code(const char *ipv4, char *output) {
    int segments[4];
    char segmentCode[4][MAX_CODE_LENGTH];
    char result[MAX_CODE_LENGTH + 1] = "";

    if (sscanf(ipv4, "%d.%d.%d.%d", &segments[0], &segments[1], &segments[2],
               &segments[3]) != 4) {
        fprintf(stderr, "Invalid IPv4 address format.\n");
        strcpy(output, "ERROR");
        return;
    }

    int totalLength = 0;

    for (int i = 0; i < 4 && totalLength < MAX_CODE_LENGTH; i++) {
        int_to_base36(segments[i], segmentCode[i], MAX_CODE_LENGTH);

        int segmentLength = strlen(segmentCode[i]);
        if (totalLength + segmentLength > MAX_CODE_LENGTH) {
            segmentCode[i][MAX_CODE_LENGTH - totalLength] =
                '\0'; // Truncate if adding exceeds length
            segmentLength = MAX_CODE_LENGTH - totalLength;
        }

        strcat(result, segmentCode[i]);
        totalLength += segmentLength;

        if (totalLength >= MAX_CODE_LENGTH)
            break;
    }

    strcpy(output, result);
}

void update_menu_state() {}

void render_menu_state() {
    DrawText("< CGAME >", (SCREEN_WIDTH / 2) - 150, 100, 50, WHITE);

    if (GuiButton((Rectangle){(SCREEN_WIDTH / 2) - (BUTTON_WIDTH / 2),
                              (SCREEN_HEIGHT / 2) - (BUTTON_HEIGHT / 2),
                              BUTTON_WIDTH, BUTTON_HEIGHT},
                  "FIND SERVER")) {
        init_server_state();
        shared.current_state = SERVER_SELECTION;
    }

    if (GuiButton((Rectangle){(SCREEN_WIDTH / 2) - (BUTTON_WIDTH / 2),
                              (SCREEN_HEIGHT / 2) + BUTTON_HEIGHT, BUTTON_WIDTH,
                              BUTTON_HEIGHT},
                  "CREATE SERVER")) {
        char *ip = get_internet_ip();
        strncpy(shared.server_ipv4, ip, sizeof(shared.server_ipv4) - 1);

        char code[6];
        ipv4_to_code(ip, code);
        strncpy(shared.server_code, code, sizeof(shared.server_code) - 1);
        shared.current_state = LOBBY;
    }
}

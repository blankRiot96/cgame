#include "menu_state.h"
#define RAYGUI_IMPLEMENTATION
#include "networking.h"
#include "raygui.h"
#include "raylib.h"
#include "shared.h"
#include <stdio.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

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
        ipv4_to_base64(ip, code);
        strncpy(shared.server_code, code, sizeof(shared.server_code) - 1);
        shared.current_state = LOBBY;
    }
}

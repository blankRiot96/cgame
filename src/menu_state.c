#include "menu_state.h"
#define RAYGUI_IMPLEMENTATION
#include "networking.h"
#include "raygui.h"
#include "raylib.h"
#include "shared.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_CODE_LENGTH 7
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 100

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

char client_name[30] = "";

void init_menu_state() { GuiSetStyle(DEFAULT, TEXT_SIZE, 32); }

void update_menu_state() {}

void render_menu_state() {
    DrawText("< CGAME >", (SCREEN_WIDTH / 2) - 150, 100, 50, WHITE);

    GuiTextBox((Rectangle){(SCREEN_WIDTH / 2) - (BUTTON_WIDTH / 2),
                           (SCREEN_HEIGHT / 2) - BUTTON_HEIGHT - 50,
                           BUTTON_WIDTH, 50},
               client_name, 30, true);

    if (GuiButton((Rectangle){(SCREEN_WIDTH / 2) - (BUTTON_WIDTH / 2),
                              (SCREEN_HEIGHT / 2) - (BUTTON_HEIGHT / 2),
                              BUTTON_WIDTH, BUTTON_HEIGHT},
                  "FIND SERVER")) {
        strncpy(shared.client_name, client_name,
                sizeof(shared.client_name) - 1);
        shared.client_name[sizeof(shared.client_name) - 1] = '\0';

        init_server_state();
        shared.current_state = SERVER_SELECTION;
    }

    if (GuiButton((Rectangle){(SCREEN_WIDTH / 2) - (BUTTON_WIDTH / 2),
                              (SCREEN_HEIGHT / 2) + BUTTON_HEIGHT, BUTTON_WIDTH,
                              BUTTON_HEIGHT},
                  "CREATE SERVER")) {
        char *ip = get_internet_ip();
        strncpy(shared.server_ipv4, ip, sizeof(shared.server_ipv4) - 1);
        shared.server_ipv4[sizeof(shared.server_ipv4) - 1] = '\0';

        char code[7]; // 6 base64 chars + 1 for the null terminator
        ipv4_to_base64(ip, code);

        // Debugging log to ensure correct base64 encoding
        printf("Base64 Encoded IP: %s\n", code);

        strncpy(shared.server_code, code, sizeof(shared.server_code) - 1);
        shared.server_code[sizeof(shared.server_code) - 1] =
            '\0'; // Null-terminate

        printf("\n\nSERVER CODE: %s\n\n", shared.server_code);

        strncpy(shared.client_name, client_name,
                sizeof(shared.client_name) - 1);
        shared.client_name[sizeof(shared.client_name) - 1] = '\0';

        printf("CLIENT NAME: %s\n");

        // Start the server in a separate thread
        pthread_t server_thread;
        if (pthread_create(&server_thread, NULL, start_server, NULL) != 0) {
            perror("Failed to create server thread");
        }

        // Start the client in a separate thread as well
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, start_client, NULL) != 0) {
            perror("Failed to create client thread");
        }

        shared.current_state = LOBBY;
    }
}

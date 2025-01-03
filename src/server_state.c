#include "server_state.h"
#include "networking.h"
#include "raygui.h"
#include "raylib.h"
#include "shared.h"
#include <pthread.h>
#include <stdio.h>

#define MAX_INPUT_LENGTH 7
#define INPUT_BOX_WIDTH 500
#define INPUT_BOX_HEIGHT 100
#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 70

char server_code[MAX_INPUT_LENGTH] = "";

void init_server_state() { GuiSetStyle(DEFAULT, TEXT_SIZE, 32); }

void update_server_state() {}

void render_server_state() {
    DrawText("Server State", 200, 200, 20, WHITE);

    GuiTextBox((Rectangle){200, 200 + 20, INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT},
               server_code, MAX_INPUT_LENGTH, true);

    if (GuiButton((Rectangle){200, 200 + 20 + INPUT_BOX_HEIGHT, BUTTON_WIDTH,
                              BUTTON_HEIGHT},
                  "JOIN")) {

        strncpy(shared.server_code, server_code,
                sizeof(shared.server_code) - 1);
        shared.server_code[sizeof(shared.server_code) - 1] = '\0';

        char ip[16];
        base64_to_ipv4(server_code, ip);
        strncpy(shared.server_ipv4, ip, sizeof(shared.server_ipv4) - 1);
        shared.server_ipv4[sizeof(shared.server_ipv4) - 1] = '\0';

        // Start the client when "JOIN" is clicked
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, start_client, NULL) != 0) {
            perror("Failed to create client thread");
        }

        shared.current_state = LOBBY;
    }
}

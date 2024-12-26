#include "lobby_state.h"
#include "raylib.h"
#include "raymath.h"
#include "shared.h"
#include <stdio.h>

const float WABBIT_SPEED = 500;

typedef struct {
    Vector2 pos;
} Wabbit;

void update_wabbit(Wabbit *wabbit) {
    float dt = GetFrameTime();
    Vector2 delta = Vector2Zero();
    delta.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    delta.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
    delta = Vector2Normalize(delta);

    wabbit->pos =
        Vector2Add(wabbit->pos, Vector2Scale(delta, WABBIT_SPEED * dt));
}

void render_wabbit(const Wabbit *wabbit) {
    // DrawTexture(wabbit->image, wabbit->pos.x, wabbit->pos.y, WHITE);
    DrawText(shared.client_name, wabbit->pos.x, wabbit->pos.y - 50, 16, GREEN);
    DrawRectangle(wabbit->pos.x, wabbit->pos.y, 50, 50, BLUE);
}

Wabbit square = (Wabbit){.pos = (Vector2){100, 100}};

void update_lobby_state() {
    update_wabbit(&square);
    shared.client_packet.square_x = square.pos.x;
    shared.client_packet.square_y = square.pos.y;
}

void render_lobby_state() {
    DrawText("LobbyState", 0, 0, 20, WHITE);
    DrawText(shared.server_code, 0, 100, 20, RED);
    render_wabbit(&square); // Draw the current client

    // Render other clients' squares and names
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (shared.other_client_names[i][0] !=
            '\0') { // Check if client name exists
            DrawText(shared.other_client_names[i],
                     shared.other_client_packets[i].square_x,
                     shared.other_client_packets[i].square_y - 50, 16, GREEN);
            DrawRectangle(shared.other_client_packets[i].square_x,
                          shared.other_client_packets[i].square_y, 50, 50,
                          BLUE);
        }
    }
}

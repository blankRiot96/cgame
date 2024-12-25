#include "lobby_state.h"
#include "raylib.h"
#include "shared.h"
#include <stdio.h>

void update_lobby_state() {}
void render_lobby_state() {
    DrawText("LobbyState", 200, 200, 20, WHITE);
    DrawText(shared.server_code, 200, 300, 20, RED);
}
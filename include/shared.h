#ifndef SHARED_H
#define SHARED_H

#include "game_state.h"
#include "lobby_state.h"
#include "menu_state.h"
#include "server_state.h"
#include "state_enums.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

typedef struct {
    State current_state;
    GameState game_state;
    MenuState menu_state;
    ServerState server_state;
    LobbyState lobby_state;

    char server_ipv4[15];
    char server_code[6];
} Shared;

extern Shared shared;

#endif

#include "shared.h"

Shared shared = {
    .current_state = MENU,
    .game_state = (GameState){},
    .menu_state = (MenuState){},
    .server_state = (ServerState){},
    .lobby_state = (LobbyState){},
};
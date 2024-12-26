
#include "shared.h"

Shared shared = {
    .current_state = MENU,
    .game_state = (GameState){},
    .menu_state = (MenuState){},
    .server_state = (ServerState){},
    .lobby_state = (LobbyState){},
    .client_packet = (Packet){.square_x = 10, .square_y = 20},
};
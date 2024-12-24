#include "state_dispatcher.h"
#include "shared.h"
#include <stdio.h>

void update_current_state() {
    switch (shared.current_state) {
    case GAME:
        update_game_state();
        break;

    case MENU:
        update_menu_state();
        break;

    default:
        break;
    }
}
void render_current_state() {
    switch (shared.current_state) {
    case GAME:
        render_game_state();
        break;

    case MENU:
        render_menu_state();
        break;

    default:
        break;
    }
}
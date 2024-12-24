#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "shared.h"
#include "state_dispatcher.h"

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Raylib");
    SearchAndSetResourceDir("resources");

    while (!WindowShouldClose()) {
        update_current_state();

        BeginDrawing();
        ClearBackground(BLACK);
        render_current_state();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

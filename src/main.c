/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty.
In no event
--will the authors be held liable for any damages arising from the use of this
software.

--Permission is granted to anyone to use this software for any purpose,
including commercial
--applications, and to alter it and redistribute it freely, subject to the
following restrictions:

--  1. The origin of this software must not be misrepresented; you must not
claim that you
--  wrote the original software. If you use this software in a product, an
acknowledgment
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be
misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

const float WABBIT_SPEED = 5;

struct Wabbit {
	Vector2 pos;
	Texture image;
};

void update_wabbit(struct Wabbit *wabbit) {
	Vector2 delta = Vector2Zero();
	delta.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	delta.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	delta = Vector2Normalize(delta);

	wabbit->pos = Vector2Add(wabbit->pos, Vector2Scale(delta, WABBIT_SPEED));
}

void render_wabbit(const struct Wabbit *wabbit) {
	DrawTexture(wabbit->image, wabbit->pos.x, wabbit->pos.y, WHITE);
}

void unload_wabbit(struct Wabbit *wabbit) { UnloadTexture(wabbit->image); }

int main() {
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 800, "Hello Raylib");
	SearchAndSetResourceDir("resources");

	struct Wabbit wabbit = {.pos = (Vector2){},
	                        .image = LoadTexture("wabbit_alpha.png")};

	while (!WindowShouldClose()) {
		// Update
		update_wabbit(&wabbit);

		// Draw
		BeginDrawing();
		ClearBackground(BLACK);

		DrawText("C programming yeahhh", 200, 200, 20, WHITE);
		render_wabbit(&wabbit);

		EndDrawing();
	}

	unload_wabbit(&wabbit);

	CloseWindow();

	return 0;
}

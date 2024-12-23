#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

const float WABBIT_SPEED = 500;

struct Wabbit {
	Vector2 pos;
	Texture image;
};

void update_wabbit(struct Wabbit *wabbit) {
	float dt = GetFrameTime();
	Vector2 delta = Vector2Zero();
	delta.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	delta.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	delta = Vector2Normalize(delta);

	wabbit->pos =
	    Vector2Add(wabbit->pos, Vector2Scale(delta, WABBIT_SPEED * dt));
}

void render_wabbit(const struct Wabbit *wabbit) {
	DrawTexture(wabbit->image, wabbit->pos.x, wabbit->pos.y, WHITE);
}

void unload_wabbit(struct Wabbit *wabbit) { UnloadTexture(wabbit->image); }

int main() {
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 800, "Hello Raylib");
	SearchAndSetResourceDir("resources");

	struct Wabbit wabbit = {.pos = (Vector2){100, 100},
	                        .image = LoadTexture("wabbit_alpha.png")};

	while (!WindowShouldClose()) {
		// Update
		update_wabbit(&wabbit);

		// Draw
		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(10, 10);
		DrawText("C programming yeahhh", 200, 200, 20, WHITE);
		render_wabbit(&wabbit);

		EndDrawing();
	}

	unload_wabbit(&wabbit);

	CloseWindow();

	return 0;
}

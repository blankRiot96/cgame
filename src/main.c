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

	Camera3D camera = {0};
	camera.position = (Vector3){10.0f, 10.0f, 10.0f}; // Camera position
	camera.target = (Vector3){0.0f, 0.0f, 0.0f};      // Camera looking at point
	camera.up = (Vector3){0.0f, 1.0f,
	                      0.0f}; // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;         // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE; // Camera projection type
	Vector3 cube_pos = {-10.0f, -10.0f, -2.0f};

	Model shape = LoadModel("shape.obj");
	Vector3 shape_pos = {0.0f, 0.0f, 0.0f};

	struct Wabbit wabbit = {.pos = (Vector2){100, 100},
	                        .image = LoadTexture("wabbit_alpha.png")};

	while (!WindowShouldClose()) {
		// Update
		UpdateCamera(&camera, CAMERA_FREE);
		// update_wabbit(&wabbit);

		// Draw
		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);
		DrawCube(cube_pos, 1.0f, 1.0f, 1.0f, RED);
		DrawCubeWires(cube_pos, 1.0f, 1.0f, 1.0f, BLUE);

		DrawModel(shape, shape_pos, 1.0, WHITE);
		DrawModelWires(shape, shape_pos, 1.0, BLACK);
		EndMode3D();

		DrawFPS(10, 10);
		DrawText("C programming yeahhh", 200, 200, 20, WHITE);
		render_wabbit(&wabbit);
		EndDrawing();
	}

	unload_wabbit(&wabbit);

	CloseWindow();

	return 0;
}

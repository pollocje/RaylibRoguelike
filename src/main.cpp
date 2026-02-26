#include "raylib.h"

const int screenWidth = 800;
const int screenHeight = 450;

int main() {

  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(30);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(BLACK);
    DrawText("semGame is running!", 190, 200, 20, GREEN);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

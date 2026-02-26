#include "raylib.h"

const int screenWidth = 800;
const int screenHeight = 450;

// ----- Function Declarations -----

// Draw Grid Function (Eventually Removed)
void drawGrid(int width, int height, int cellW, int cellH);

int main() {

  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(30);

  while (!WindowShouldClose()) {

    BeginDrawing();

    ClearBackground(BLACK);
    drawGrid(screenWidth, screenHeight, 80, 45);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

// Draw Grid Function (Eventually Removed)
// Only for development, won't need visual grid for final game
void drawGrid(int width, int height, int cellW, int cellH) {
  // two For loops

  // Vertical Lines
  for (int i = 0; i < width; i += cellW) {
    DrawLine(i + cellW, 0, i + cellW, height, GREEN);
  }

  // Horizontal Lines
  for (int j = 0; j < height; j += cellH) {
    DrawLine(0, j + cellH, width, j + cellH, GREEN);
  }
}

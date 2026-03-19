#include "Map.h"
#include "Player.h"

#include "raylib.h"

const int screenWidth = 1280;
const int screenHeight = 720;

int main() {

  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(30);

  // Create Player on Start
  Player player;

  // Create Map
  Map map(screenWidth, screenHeight);

  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_SPACE)) {
      map.generate();
    }

    BeginDrawing();

    ClearBackground(BLACK);
    player.drawPlayer();

    map.drawMap();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

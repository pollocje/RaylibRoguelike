#include "MainMenu.h"
#include "Map.h"
#include "Player.h"

#include "raylib.h"

const int screenWidth = 1280;
const int screenHeight = 720;

// GameState Enum
enum GameState { MENU, PLAYING };

int main() {

  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(60);

  // GameState
  bool isGameStarted = false;

  // MainMenu Init
  MainMenu mainMenu;

  // Create Player on Start
  Player player;

  // Create Map
  Map map(screenWidth, screenHeight);

  while (!WindowShouldClose()) {
      // --- 1. UPDATE PHASE (The Brain) ---
      if (!isGameStarted) {
          MainMenu::MainMenuButtons selection = mainMenu.Update();
          if (selection == MainMenu::START) isGameStarted = true;
          if (selection == MainMenu::EXIT)  break;
      }
      else {
          // Game-only logic
          if (IsKeyPressed(KEY_SPACE)) map.generate();
          player.movement(map);
      }

      // --- 2. DRAW PHASE (The Eyes) ---
      BeginDrawing();
      ClearBackground(BLACK);

      if (!isGameStarted) {
          mainMenu.Draw();
      }
      else {
          // Only draw these when the game is actually running
          map.drawMap();
          player.drawPlayer();
          DrawText("Game is Running!", 10, 10, 20, GREEN);
      }
      EndDrawing();
  }

  CloseWindow();
  return 0;
}

#include "MainMenu.h"
#include "Map.h"
#include "Player.h"
#include "Transition.h"

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

  // Generate the Map
  Map gameMap(screenWidth, screenHeight);

  // Create Player on Start
  Player player;

  // Spawn Player on Map
  player.spawn(gameMap);

  // Initial Stairs Spawn
  gameMap.spawnStairs();

  // Transition Init
  Transition transition;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- 1. UPDATE PHASE (The Brain) ---
    if (!isGameStarted) {
      MainMenu::MainMenuButtons selection = mainMenu.Update();
      if (selection == MainMenu::START)
        isGameStarted = true;
      if (selection == MainMenu::EXIT)
        break;
    } else {
      transition.Update(dt);

      if (!transition.IsActive()) {
        // Game-only logic
        if (IsKeyPressed(KEY_SPACE)) {
          gameMap.generate();
          player.spawn(gameMap);
          gameMap.spawnStairs();
        }

        // Check for Stairs Collision
        if (player.getGridX() == gameMap.stairsX &&
            player.getGridY() == gameMap.stairsY) {
          transition.Start("The descent continues...");
        }

        player.movement(gameMap);
      } else {
          // Transition is active, check if we should swap the map
          if (transition.ShouldSwapMap()) {
              gameMap.generate();
              // Force the player's current spot to be a floor tile
              gameMap.forceFloor(player.getGridX(), player.getGridY());
              // Move stairs to a new location
              gameMap.spawnStairs();
          }
      }
    }

    // --- 2. DRAW PHASE (The Eyes) ---
    BeginDrawing();
    ClearBackground(BLACK);

    if (!isGameStarted) {
      mainMenu.Draw();
    } else {
      // Only draw these when the game is actually running
      gameMap.drawMap();
      gameMap.drawStairs(); // NEW: Draw Stairs
      player.drawPlayer();
      DrawText("Game is Running!", 10, 10, 20, GREEN);

      // Draw transition overlay
      transition.Draw();
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

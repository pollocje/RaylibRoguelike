// typing stuff here i wonder if it will appear in visual studio
#include "MainMenu.h"
#include "Map.h"
#include "Modifiers.h"
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

  // Generate the Map
  Map gameMap(screenWidth, screenHeight);

  // Create Player on Start
  Player player;

  // Spawn Player on Map
  player.spawn(gameMap);

  // Initial Stairs Spawn
  gameMap.spawnStairs();

  while (!WindowShouldClose()) {
    // --- 1. UPDATE PHASE (The Brain) ---
    if (!isGameStarted) {
      MainMenu::MainMenuButtons selection = mainMenu.Update();
      if (selection == MainMenu::START)
        isGameStarted = true;
      if (selection == MainMenu::EXIT)
        break;
    } else {
      // Game-only logic
      if (IsKeyPressed(KEY_SPACE)) {
        gameMap.generate();
        player.spawn(gameMap);
        gameMap.spawnStairs();
      }

      // Check for Stairs Collision
      if (player.getGridX() == gameMap.stairsX &&
          player.getGridY() == gameMap.stairsY) {

        gameMap.generate();
        // Force the player's current spot to be a floor tile
        gameMap.forceFloor(player.getGridX(), player.getGridY());
        // Move stairs to a new location
        gameMap.spawnStairs();
      }

      player.movement(gameMap);
    }
    // Testing buff functionality
    if (IsKeyPressed(KEY_ONE)) {
        player.ApplyModifier({ ModifierType::Speed, ModifierTier::Tier1 });
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_TWO)) {
        player.ApplyModifier({ ModifierType::Health, ModifierTier::Tier2 });
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_THREE)) {
        player.ApplyModifier({ ModifierType::FireRate, ModifierTier::Tier3 });
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_FOUR)) {
        player.ApplyModifier({ ModifierType::Luck, ModifierTier::Tier1 });
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_FIVE)) {
        player.ApplyModifier({ ModifierType::Dodge, ModifierTier::Tier2 });
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_SIX)) {
        player.ApplyModifier({ ModifierType::XPGain, ModifierTier::Tier3 });
        player.PrintStats();
    }

    // Gameplay method testing
    if (IsKeyPressed(KEY_H)) {
        player.TakeDamage(20);
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_J)) {
        player.Heal(15);
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_K)) {
        player.GainXP(100);
        player.PrintStats();
    }

    if (IsKeyPressed(KEY_L)) {
        int damage = player.GetDamageOutput(10);
        TraceLog(LOG_INFO, TextFormat("Player dealt %d damage", damage));
    }

    player.movement(map);

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
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

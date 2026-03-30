#include "MainMenu.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Transition.h"

#include "raylib.h"
#include <vector>
#include <string>

const int screenWidth  = 1280;
const int screenHeight = 720;

enum GameState { MENU, PLAYING };

// Spawn enemies only within the player's reachable region
void spawnEnemies(std::vector<Enemy> &enemies, Map &gameMap, int floor,
                  int playerX, int playerY, std::vector<Vector2> &reachable) {
  enemies.clear();
  if (reachable.empty()) return;

  int count = 3 + (floor / 3); // more enemies on deeper floors
  for (int i = 0; i < count; i++) {
    for (int attempt = 0; attempt < 200; attempt++) {
      int idx = rand() % (int)reachable.size();
      int ex = (int)reachable[idx].x;
      int ey = (int)reachable[idx].y;

      // Don't spawn on the player
      if (ex == playerX && ey == playerY) continue;

      // Don't spawn on the stairs
      if (ex == gameMap.stairsX && ey == gameMap.stairsY) continue;

      // Don't spawn on another enemy
      bool conflict = false;
      for (int j = 0; j < (int)enemies.size(); j++) {
        if (enemies[j].gridX == ex && enemies[j].gridY == ey) {
          conflict = true;
          break;
        }
      }
      if (conflict) continue;

      enemies.push_back(Enemy(ex, ey, floor));
      break;
    }
  }
}

int main() {
  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(60);

  bool isGameStarted = false;
  int currentFloor = 1;

  MainMenu mainMenu;

  Map gameMap(screenWidth, screenHeight);
  Player player;
  player.spawn(gameMap);

  std::vector<Vector2> reachable; // reachable tiles from player spawn — rebuilt each map generate
  reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
  gameMap.spawnStairsInRegion(reachable);

  std::vector<Enemy> enemies;
  spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);

  Transition transition;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- UPDATE ---
    if (!isGameStarted) {
      MainMenu::MainMenuButtons selection = mainMenu.Update();
      if (selection == MainMenu::START) isGameStarted = true;
      if (selection == MainMenu::EXIT)  break;
    } else {
      transition.Update(dt);

      if (!transition.IsActive()) {

        if (IsKeyPressed(KEY_SPACE)) {
          gameMap.generate();
          player.spawn(gameMap);
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable);
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
        }

        // Stairs collision
        if (player.getGridX() == gameMap.stairsX &&
            player.getGridY() == gameMap.stairsY) {
          transition.Start("The descent continues...");
        }

        // Player takes their turn
        bool playerActed = player.movement(gameMap, enemies);

        // Enemies take their turn after the player acts
        if (playerActed) {
          for (int i = 0; i < (int)enemies.size(); i++) {
            if (enemies[i].isAlive()) {
              int dmg = enemies[i].takeTurn(
                player.getGridX(), player.getGridY(), gameMap, enemies);
              player.applyDamage(dmg);
            }
          }

          // Remove dead enemies
          std::vector<Enemy> alive;
          for (int i = 0; i < (int)enemies.size(); i++) {
            if (enemies[i].isAlive()) alive.push_back(enemies[i]);
          }
          enemies = alive;
        }

      } else {
        // Transition is active — swap the map when ready
        if (transition.ShouldSwapMap()) {
          currentFloor++;
          gameMap.generate();
          gameMap.forceFloor(player.getGridX(), player.getGridY());
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable);
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
        }
      }
    }

    // --- DRAW ---
    BeginDrawing();
    ClearBackground(BLACK);

    if (!isGameStarted) {
      mainMenu.Draw();
    } else {
      gameMap.drawMap();
      gameMap.drawStairs();

      // Draw enemies
      for (int i = 0; i < (int)enemies.size(); i++) {
        enemies[i].draw();
      }

      player.drawPlayer();

      // HUD: floor and player HP
      std::string floorText = "Floor: " + std::to_string(currentFloor);
      DrawText(floorText.c_str(), 10, 10, 20, WHITE);

      std::string hpText = "HP: " + std::to_string(player.getHealth())
                         + " / " + std::to_string(player.getMaxHealth());
      DrawText(hpText.c_str(), 10, 35, 20, RED);

      transition.Draw();
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

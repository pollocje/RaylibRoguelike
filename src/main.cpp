#include "MainMenu.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Transition.h"
#include "Item.h"

#include "raylib.h"
#include <vector>
#include <string>

const int screenWidth  = 1280;
const int screenHeight = 720;

enum GameState { MENU, PLAYING, GAME_OVER };

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

void spawnItems(std::vector<FloorItem> &items, Map &gameMap,
                int playerX, int playerY, std::vector<Vector2> &reachable) {
    items.clear();
    if (reachable.empty()) return;

    ItemType possible[3] = { ITEM_HEALTH_POTION, ITEM_FIREBALL_SCROLL, ITEM_FREEZE_SCROLL };
    int count = 1 + rand() % 5; // 1-5 items per floor

    for (int i = 0; i < count; i++) {
        for (int attempt = 0; attempt < 200; attempt++) {
            int idx = rand() % (int)reachable.size();
            int ix = (int)reachable[idx].x;
            int iy = (int)reachable[idx].y;

            if (ix == playerX && iy == playerY) continue;
            if (ix == gameMap.stairsX && iy == gameMap.stairsY) continue;

            bool conflict = false;
            for (int j = 0; j < (int)items.size(); j++) {
                if (items[j].gridX == ix && items[j].gridY == iy) {
                    conflict = true;
                    break;
                }
            }
            if (conflict) continue;

            ItemType t = possible[rand() % 3];
            items.push_back(FloorItem(t, ix, iy));
            break;
        }
    }
}

int main() {
  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(60);

  GameState gameState = MENU;
  int currentFloor = 1;
  int deathFloor = 1;
  float gameOverAlpha = 0.0f;

  std::vector<FloorItem> floorItems;
  bool isTargeting   = false;
  int  targetingSlot = -1;
  int  targetIndex   = 0;

  MainMenu mainMenu;

  Map gameMap(screenWidth, screenHeight);
  Player player;
  player.spawn(gameMap);

  std::vector<Vector2> reachable; // reachable tiles from player spawn — rebuilt each map generate
  reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
  gameMap.spawnStairsInRegion(reachable);

  std::vector<Enemy> enemies;
  spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
  spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);

  Transition transition;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- UPDATE ---
    if (gameState == MENU) {
      MainMenu::MainMenuButtons selection = mainMenu.Update();
      if (selection == MainMenu::START) gameState = PLAYING;
      if (selection == MainMenu::EXIT)  break;
    } else if (gameState == GAME_OVER) {
      if (gameOverAlpha < 1.0f) gameOverAlpha += dt * 1.5f;
      if (gameOverAlpha > 1.0f) gameOverAlpha = 1.0f;

      if (IsKeyPressed(KEY_ENTER)) {
        // Reset everything back to a fresh game
        gameState    = MENU;
        currentFloor = 1;
        isTargeting  = false;
        gameMap.generate();
        player = Player();
        player.spawn(gameMap);
        reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
        gameMap.spawnStairsInRegion(reachable);
        spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
        spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
      }
    } else {
      transition.Update(dt);

      if (!transition.IsActive()) {

        if (IsKeyPressed(KEY_SPACE)) {
          gameMap.generate();
          player.spawn(gameMap);
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable);
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
          spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
          isTargeting = false;
        }

        // Stairs collision
        if (player.getGridX() == gameMap.stairsX &&
            player.getGridY() == gameMap.stairsY) {
          isTargeting = false;
          transition.Start("The descent continues...");
        }

        if (isTargeting) {
          // Arrow keys cycle through enemies
          if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN)) {
            targetIndex = (targetIndex + 1) % (int)enemies.size();
          }
          if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP)) {
            targetIndex = (targetIndex - 1 + (int)enemies.size()) % (int)enemies.size();
          }

          // Confirm cast with same number key that opened targeting
          int confirmKeys[3] = { KEY_ONE, KEY_TWO, KEY_THREE };
          if (IsKeyPressed(confirmKeys[targetingSlot])) {
            ItemType item = player.inventory.slots[targetingSlot];

            if (item == ITEM_FIREBALL_SCROLL) {
              enemies[targetIndex].takeDamage(25);
            } else if (item == ITEM_FREEZE_SCROLL) {
              enemies[targetIndex].applyFreeze(20);
            }

            player.inventory.removeSlot(targetingSlot);
            isTargeting = false;

            // Remove killed enemies
            std::vector<Enemy> alive;
            for (int i = 0; i < (int)enemies.size(); i++) {
              if (enemies[i].isAlive()) alive.push_back(enemies[i]);
            }
            enemies = alive;
          }

          if (IsKeyPressed(KEY_ESCAPE)) {
            isTargeting = false;
          }

        } else {
          // H key: use health potion
          if (IsKeyPressed(KEY_H)) {
            if (player.inventory.useHealthPotion()) {
              player.heal(30);
            }
          }

          // Number keys 1/2/3: use general inventory slots
          int slotKeys[3] = { KEY_ONE, KEY_TWO, KEY_THREE };
          for (int i = 0; i < 3; i++) {
            if (IsKeyPressed(slotKeys[i]) && player.inventory.slots[i] != ITEM_NONE) {
              if (player.inventory.requiresTargeting(i)) {
                if (!enemies.empty()) {
                  isTargeting   = true;
                  targetingSlot = i;
                  targetIndex   = 0;
                }
              }
              // instant-use items would go here (none yet)
            }
          }

          // Auto-pickup: check if player is standing on a floor item
          for (int i = 0; i < (int)floorItems.size(); i++) {
            if (!floorItems[i].pickedUp &&
                floorItems[i].gridX == player.getGridX() &&
                floorItems[i].gridY == player.getGridY()) {
              if (player.inventory.addItem(floorItems[i].type)) {
                floorItems[i].pickedUp = true;
              }
            }
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
                if (!player.isAlive()) {
                  deathFloor    = currentFloor;
                  gameState     = GAME_OVER;
                  gameOverAlpha = 0.0f;
                  break;
                }
              }
            }

            // Remove dead enemies
            std::vector<Enemy> alive;
            for (int i = 0; i < (int)enemies.size(); i++) {
              if (enemies[i].isAlive()) alive.push_back(enemies[i]);
            }
            enemies = alive;
          }
        }

      } else {
        // Transition is active — swap the map when ready
        if (transition.ShouldSwapMap()) {
          currentFloor++;
          isTargeting = false;
          gameMap.generate();
          gameMap.forceFloor(player.getGridX(), player.getGridY());
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable);
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
          spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
        }
      }
    }

    // --- DRAW ---
    BeginDrawing();
    ClearBackground(BLACK);

    if (gameState == MENU) {
      mainMenu.Draw();
    } else if (gameState == GAME_OVER) {
      DrawText("YOU DIED",
        screenWidth / 2 - MeasureText("YOU DIED", 80) / 2,
        screenHeight / 2 - 100, 80, Fade(RED, gameOverAlpha));

      std::string reachedText = "Reached floor " + std::to_string(deathFloor);
      DrawText(reachedText.c_str(),
        screenWidth / 2 - MeasureText(reachedText.c_str(), 30) / 2,
        screenHeight / 2 + 10, 30, Fade(WHITE, gameOverAlpha));

      DrawText("Press ENTER to return to menu",
        screenWidth / 2 - MeasureText("Press ENTER to return to menu", 20) / 2,
        screenHeight / 2 + 60, 20, Fade(GRAY, gameOverAlpha));
    } else {
      gameMap.drawMap();
      gameMap.drawStairs();

      // Draw floor items
      for (int i = 0; i < (int)floorItems.size(); i++) {
        if (!floorItems[i].pickedUp) {
          drawFloorItem(floorItems[i].type, floorItems[i].gridX, floorItems[i].gridY);
        }
      }

      // Draw enemies
      for (int i = 0; i < (int)enemies.size(); i++) {
        enemies[i].draw();
      }

      // Draw targeting highlight
      if (isTargeting && !enemies.empty()) {
        int tx = enemies[targetIndex].gridX * 40 + 20;
        int ty = enemies[targetIndex].gridY * 40 + 20;
        DrawCircleLines(tx, ty, 18, YELLOW);

        std::string castItem = getItemName(player.inventory.slots[targetingSlot]);
        std::string prompt   = "Casting: " + castItem + "  |  Arrows: cycle  |  [same key]: cast  |  ESC: cancel";
        int tw = MeasureText(prompt.c_str(), 18);
        DrawText(prompt.c_str(), screenWidth / 2 - tw / 2, 10, 18, YELLOW);
      }

      player.drawPlayer();

      // HUD: floor and player HP
      std::string floorText = "Floor: " + std::to_string(currentFloor);
      DrawText(floorText.c_str(), 10, 10, 20, WHITE);

      std::string hpText = "HP: " + std::to_string(player.getHealth())
                         + " / " + std::to_string(player.getMaxHealth());
      DrawText(hpText.c_str(), 10, 35, 20, RED);

      // HUD: inventory bar at the bottom
      DrawText(("[H] HP Potion x" + std::to_string(player.inventory.healthPotions)
               + "/" + std::to_string(Inventory::MAX_POTIONS)).c_str(),
               10, screenHeight - 55, 18, PINK);

      int slotLabels[3] = { KEY_ONE, KEY_TWO, KEY_THREE };
      std::string slotPrefix[3] = { "[1] ", "[2] ", "[3] " };
      for (int i = 0; i < 3; i++) {
        std::string slotText;
        if (player.inventory.slots[i] == ITEM_NONE) {
          slotText = slotPrefix[i] + "---";
        } else {
          slotText = slotPrefix[i] + getItemName(player.inventory.slots[i]);
        }
        Color slotColor = (isTargeting && targetingSlot == i) ? YELLOW : WHITE;
        DrawText(slotText.c_str(), 10 + i * 280, screenHeight - 30, 18, slotColor);
      }

      transition.Draw();
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

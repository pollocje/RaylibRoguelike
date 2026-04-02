#include "MainMenu.h"
#include "Map.h"
#include "Modifiers.h"
#include "Player.h"
#include "Enemy.h"
#include "Transition.h"
#include "Item.h"
#include "CombatLog.h"
#include "GameFont.h"

#include "raylib.h"
#include <vector>
#include <string>

Font gFont; // definition — extern declared in GameFont.h

const int screenWidth  = 1280;
const int screenHeight = 720;
const int PANEL_WIDTH  = 280;
const int MAP_AREA_W   = screenWidth - PANEL_WIDTH; // 1000px → 25 cols @ 40px

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

      if (ex == playerX && ey == playerY) continue;
      if (ex == gameMap.stairsX && ey == gameMap.stairsY) continue;

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

// Draw the right-side panel: stats, inventory, combat log
void drawPanel(int floor, Player &player, bool isTargeting, int targetingSlot,
               CombatLog &combatLog) {
    int px = MAP_AREA_W; // panel left edge

    // Dark background + separator
    DrawRectangle(px, 0, PANEL_WIDTH, screenHeight, Color{18, 18, 18, 255});
    DrawLine(px, 0, px, screenHeight, DARKGRAY);

    // --- Title / Floor ---
    DrawTextEx(gFont, "semGame", {(float)(px + 8), 10.0f}, 16, 1.0f, WHITE);
    std::string floorText = "Floor: " + std::to_string(floor);
    DrawTextEx(gFont, floorText.c_str(), {(float)(px + 8), 34.0f}, 12, 1.0f, GRAY);

    DrawLine(px, 56, px + PANEL_WIDTH, 56, DARKGRAY);

    // --- HP Bar ---
    int hp    = player.getHealth();
    int maxHp = player.getMaxHealth();
    std::string hpText = "HP: " + std::to_string(hp) + " / " + std::to_string(maxHp);
    DrawTextEx(gFont, hpText.c_str(), {(float)(px + 8), 64.0f}, 12, 1.0f, RED);

    // Bar background + fill
    int barX = px + 8;
    int barY = 84;
    int barW = PANEL_WIDTH - 16;
    int barH = 10;
    DrawRectangle(barX, barY, barW, barH, DARKGRAY);
    int fillW = (int)((float)hp / (float)maxHp * barW);
    if (fillW > 0)
        DrawRectangle(barX, barY, fillW, barH, RED);
    DrawRectangleLines(barX, barY, barW, barH, GRAY);

    DrawLine(px, 104, px + PANEL_WIDTH, 104, DARKGRAY);

    // --- Inventory ---
    DrawTextEx(gFont, "Inventory", {(float)(px + 8), 110.0f}, 12, 1.0f, GRAY);

    std::string hpPotText = "[H] HP Potion  x"
        + std::to_string(player.inventory.healthPotions)
        + "/" + std::to_string(Inventory::MAX_POTIONS);
    DrawTextEx(gFont, hpPotText.c_str(), {(float)(px + 8), 128.0f}, 10, 1.0f, PINK);

    std::string slotPrefix[3] = { "[1] ", "[2] ", "[3] " };
    for (int i = 0; i < 3; i++) {
        std::string slotText;
        if (player.inventory.slots[i] == ITEM_NONE)
            slotText = slotPrefix[i] + "---";
        else
            slotText = slotPrefix[i] + getItemName(player.inventory.slots[i]);

        Color slotColor = (isTargeting && targetingSlot == i) ? YELLOW : WHITE;
        DrawTextEx(gFont, slotText.c_str(), {(float)(px + 8), (float)(146 + i * 18)}, 10, 1.0f, slotColor);
    }

    DrawLine(px, 204, px + PANEL_WIDTH, 204, DARKGRAY);

    // --- Combat Log ---
    combatLog.draw(px, 210, PANEL_WIDTH);
}

int main() {
  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(60);
  gFont = LoadFont("assets/fonts/PressStart2P.ttf");

  GameState gameState = MENU;
  int currentFloor = 1;
  int deathFloor   = 1;
  float gameOverAlpha = 0.0f;

  std::vector<FloorItem> floorItems;
  bool isTargeting   = false;
  int  targetingSlot = -1;
  int  targetIndex   = 0;

  CombatLog combatLog;

  MainMenu mainMenu;

  Map gameMap(MAP_AREA_W, screenHeight); // 25x18 — leaves room for panel
  Player player;
  player.spawn(gameMap);

  std::vector<Vector2> reachable;
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
        gameState    = MENU;
        currentFloor = 1;
        isTargeting  = false;
        combatLog.clear();
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
          if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN)) {
            targetIndex = (targetIndex + 1) % (int)enemies.size();
          }
          if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP)) {
            targetIndex = (targetIndex - 1 + (int)enemies.size()) % (int)enemies.size();
          }

          int confirmKeys[3] = { KEY_ONE, KEY_TWO, KEY_THREE };
          if (IsKeyPressed(confirmKeys[targetingSlot])) {
            ItemType item = player.inventory.slots[targetingSlot];

            if (item == ITEM_FIREBALL_SCROLL) {
              enemies[targetIndex].takeDamage(25);
              combatLog.add("Fireball hits for 25 damage!");
            } else if (item == ITEM_FREEZE_SCROLL) {
              enemies[targetIndex].applyFreeze(20);
              combatLog.add("Enemy is frozen for 20 turns!");
            }

            player.inventory.removeSlot(targetingSlot);
            isTargeting = false;

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
              combatLog.add("You drink a potion, restoring 30 HP");
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
            }
          }

          // Auto-pickup
          for (int i = 0; i < (int)floorItems.size(); i++) {
            if (!floorItems[i].pickedUp &&
                floorItems[i].gridX == player.getGridX() &&
                floorItems[i].gridY == player.getGridY()) {
              if (player.inventory.addItem(floorItems[i].type)) {
                floorItems[i].pickedUp = true;
                combatLog.add("Picked up " + getItemName(floorItems[i].type));
              }
            }
          }

          // Player turn
          int playerResult = player.movement(gameMap, enemies);

          if (playerResult != 0) {
            if (playerResult > 0) {
              combatLog.add("You hit for " + std::to_string(playerResult) + " damage");
            }

            // Enemies take their turn
            for (int i = 0; i < (int)enemies.size(); i++) {
              if (enemies[i].isAlive()) {
                int dmg = enemies[i].takeTurn(
                  player.getGridX(), player.getGridY(), gameMap, enemies);
                if (dmg > 0) {
                  player.applyDamage(dmg);
                  combatLog.add("Enemy hits you for " + std::to_string(dmg));
                  if (!player.isAlive()) {
                    deathFloor    = currentFloor;
                    gameState     = GAME_OVER;
                    gameOverAlpha = 0.0f;
                    break;
                  }
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
        // Transition is active — swap map when ready
        if (transition.ShouldSwapMap()) {
          currentFloor++;
          isTargeting = false;
          combatLog.clear();
          combatLog.add("You descend to floor " + std::to_string(currentFloor));
          gameMap.generate();
          gameMap.forceFloor(player.getGridX(), player.getGridY());
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable);
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
          spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
        }
      }
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

    // --- 2. DRAW PHASE (The Eyes) ---
    BeginDrawing();
    ClearBackground(BLACK);

    if (gameState == MENU) {
      mainMenu.Draw();
    } else if (gameState == GAME_OVER) {
      const char* diedStr = "YOU DIED";
      int diedW = (int)MeasureTextEx(gFont, diedStr, 64, 1.0f).x;
      DrawTextEx(gFont, diedStr,
        {(float)(screenWidth / 2 - diedW / 2), (float)(screenHeight / 2 - 80)},
        64, 1.0f, Fade(RED, gameOverAlpha));

      std::string reachedText = "Reached floor " + std::to_string(deathFloor);
      int reachedW = (int)MeasureTextEx(gFont, reachedText.c_str(), 20, 1.0f).x;
      DrawTextEx(gFont, reachedText.c_str(),
        {(float)(screenWidth / 2 - reachedW / 2), (float)(screenHeight / 2 + 10)},
        20, 1.0f, Fade(WHITE, gameOverAlpha));

      const char* enterStr = "Press ENTER to return to menu";
      int enterW = (int)MeasureTextEx(gFont, enterStr, 14, 1.0f).x;
      DrawTextEx(gFont, enterStr,
        {(float)(screenWidth / 2 - enterW / 2), (float)(screenHeight / 2 + 50)},
        14, 1.0f, Fade(GRAY, gameOverAlpha));
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
        int tw = (int)MeasureTextEx(gFont, prompt.c_str(), 12, 1.0f).x;
        int promptX = MAP_AREA_W / 2 - tw / 2;
        if (promptX < 0) promptX = 0;
        DrawTextEx(gFont, prompt.c_str(), {(float)promptX, 10.0f}, 12, 1.0f, YELLOW);
      }

      player.drawPlayer();

      // Right-side panel
      drawPanel(currentFloor, player, isTargeting, targetingSlot, combatLog);

      transition.Draw();
    }

    EndDrawing();
  }

  UnloadFont(gFont);
  CloseWindow();
  return 0;
}

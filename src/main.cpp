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
#include <cstdlib>

Font gFont; // definition — extern declared in GameFont.h

const int screenWidth  = 1280;
const int screenHeight = 720;
const int PANEL_WIDTH  = 280;
const int MAP_AREA_W   = screenWidth - PANEL_WIDTH; // 1000px → 25 cols @ 40px

enum GameState { MENU, CHARACTER_SELECT, PLAYING, MODIFIER_SELECT, GAME_OVER, WIN_ROOM };

const int WIN_CHEST_X        = 19;
const int WIN_CHEST_Y        = 8;
const int WIN_PLAYER_START_X = 3;
const int WIN_PLAYER_START_Y = 8;

std::vector<Modifier> generateModifierOffers() {
    ModifierType allTypes[] = {
        ModifierType::Speed, ModifierType::Health,
        ModifierType::Luck,  ModifierType::Dodge,  ModifierType::XPGain
    };
    // Shuffle types via Fisher-Yates
    for (int i = 4; i > 0; i--) {
        int j = rand() % (i + 1);
        ModifierType tmp = allTypes[i]; allTypes[i] = allTypes[j]; allTypes[j] = tmp;
    }
    std::vector<Modifier> offers;
    for (int i = 0; i < 3; i++) {
        int tierRoll = rand() % 3;
        ModifierTier tier = (tierRoll == 0) ? ModifierTier::Tier1
                          : (tierRoll == 1) ? ModifierTier::Tier2
                                            : ModifierTier::Tier3;
        offers.push_back({ allTypes[i], tier });
    }
    return offers;
}

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

    ItemType possible[5] = { ITEM_HEALTH_POTION, ITEM_FIREBALL_SCROLL, ITEM_FREEZE_SCROLL, ITEM_TELEPORT_SCROLL, ITEM_RAGE_POTION };
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

            ItemType t = possible[rand() % 5];
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

    // --- Stats ---
    // Combat log: 18px header + 12 * 16px messages = 210px → bottom at 420
    int statsY = 428;
    DrawLine(px, statsY - 4, px + PANEL_WIDTH, statsY - 4, DARKGRAY);
    DrawTextEx(gFont, "Stats", {(float)(px + 8), (float)statsY}, 12, 1.0f, GRAY);
    statsY += 18;

    auto drawStat = [&](const char* label, float value, const char* suffix, Color col) {
        std::string text = std::string(label) + std::to_string((int)(value)) + suffix;
        DrawTextEx(gFont, text.c_str(), {(float)(px + 8), (float)statsY}, 10, 1.0f, col);
        statsY += 16;
    };

    drawStat("Speed Bonus:  +", player.GetSpeedBonusChance() * 100.0f, "%", SKYBLUE);
    drawStat("Dodge:         ", player.GetDodgeChance()      * 100.0f, "%", GREEN);
    drawStat("Luck:          ", player.GetLuckChance()        * 100.0f, "%", GOLD);
    drawStat("XP Gain:      +", (player.GetXPGainMultiplier() - 1.0f) * 100.0f, "%", YELLOW);

    if (player.IsRaging()) {
        std::string rageText = "Rage:          " + std::to_string(player.GetRageTurnsRemaining()) + " turns";
        DrawTextEx(gFont, rageText.c_str(), {(float)(px + 8), (float)statsY}, 10, 1.0f, ORANGE);
    }
}

int main() {
  InitWindow(screenWidth, screenHeight, "semGame");
  InitAudioDevice();
  SetTargetFPS(60);
  gFont = LoadFont("assets/fonts/PressStart2P.ttf");

  Sound sndHitHurt  = LoadSound("assets/sfx/hitHurt.wav");
  Sound sndTakeDmg  = LoadSound("assets/sfx/takeDmg.wav");
  Sound sndSpell    = LoadSound("assets/sfx/spellCast.wav");
  Sound sndPickUp   = LoadSound("assets/sfx/pickUp.wav");
  Sound sndDescend  = LoadSound("assets/sfx/descend.wav");
  Sound sndDeath    = LoadSound("assets/sfx/death.wav");

  GameState gameState = MENU;
  int currentFloor = 1;
  int deathFloor   = 1;
  float gameOverAlpha = 0.0f;

  std::vector<FloorItem> floorItems;
  int  characterSelectIndex  = 0;
  std::vector<Modifier> offeredModifiers;
  int  modifierSelectIndex   = 0;
  bool pendingModifierSelect = false;
  bool winChestPickedUp = false;
  bool playerOnStairs   = false;
  int  pendingPlayerActions  = 1;
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
  gameMap.spawnStairsInRegion(reachable, player.getGridX(), player.getGridY());

  std::vector<Enemy> enemies;
  spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
  spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);

  Transition transition;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- UPDATE ---
    if (gameState == MENU) {
      MainMenu::MainMenuButtons selection = mainMenu.Update();
      if (selection == MainMenu::START) { characterSelectIndex = 0; gameState = CHARACTER_SELECT; }
      if (selection == MainMenu::EXIT)  break;
    } else if (gameState == CHARACTER_SELECT) {
      if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A))
        characterSelectIndex = (characterSelectIndex - 1 + 3) % 3;
      if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        characterSelectIndex = (characterSelectIndex + 1) % 3;
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        // Apply chosen class stats to player
        // hpMult, atkMult, dodgeMult, spdMult, isMage
        if      (characterSelectIndex == 0) player.ApplyCharacterClass(2.0f, 1.0f,  1.25f, 1.0f,  false); // Warrior
        else if (characterSelectIndex == 1) player.ApplyCharacterClass(1.0f, 1.5f,  2.0f,  1.5f,  false); // Rogue
        else                                player.ApplyCharacterClass(0.5f, 0.75f, 1.0f,  1.0f,  true);  // Mage
        gameState = PLAYING;
      }
    } else if (gameState == GAME_OVER) {
      if (gameOverAlpha < 1.0f) gameOverAlpha += dt * 1.5f;
      if (gameOverAlpha > 1.0f) gameOverAlpha = 1.0f;

      if (IsKeyPressed(KEY_ENTER)) {
        currentFloor = 1;
        isTargeting  = false;
        pendingPlayerActions  = 1;
        characterSelectIndex  = 0;
        combatLog.clear();
        gameMap.generate();
        player = Player();
        player.spawn(gameMap);
        reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
        gameMap.spawnStairsInRegion(reachable, player.getGridX(), player.getGridY());
        spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
        spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
        gameState = CHARACTER_SELECT;
      }
    } else if (gameState == PLAYING) {
      transition.Update(dt);

      if (!transition.IsActive()) {

        if (pendingModifierSelect) {
          pendingModifierSelect = false;
          gameState = MODIFIER_SELECT;
        } else {

        if (IsKeyPressed(KEY_SPACE)) {
          gameMap.generate();
          player.spawn(gameMap);
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable, player.getGridX(), player.getGridY());
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
          spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
          isTargeting = false;
        }

        // Stairs collision
        bool onStairs = (player.getGridX() == gameMap.stairsX &&
                         player.getGridY() == gameMap.stairsY);
        if (onStairs && !playerOnStairs) {
          if (!enemies.empty()) {
            combatLog.add("Must defeat all enemies first!");
          } else {
            isTargeting = false;
            PlaySound(sndDescend);
            transition.Start("The descent continues...");
          }
        }
        playerOnStairs = onStairs;

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
              PlaySound(sndSpell);
            } else if (item == ITEM_FREEZE_SCROLL) {
              enemies[targetIndex].applyFreeze(20);
              combatLog.add("Enemy is frozen for 20 turns!");
              PlaySound(sndSpell);
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
              PlaySound(sndSpell);
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
              } else if (player.inventory.slots[i] == ITEM_TELEPORT_SCROLL) {
                int idx = rand() % (int)reachable.size();
                player.setGridPosition((int)reachable[idx].x, (int)reachable[idx].y);
                player.inventory.removeSlot(i);
                combatLog.add("You vanish in a purple mist!");
                PlaySound(sndSpell);
              } else if (player.inventory.slots[i] == ITEM_RAGE_POTION) {
                player.ApplyRage(10);
                player.inventory.removeSlot(i);
                combatLog.add("Rage courses through you! (10 turns)");
                PlaySound(sndSpell);
              }
            }
          }

          // Auto-pickup
          for (int i = 0; i < (int)floorItems.size(); i++) {
            if (!floorItems[i].pickedUp &&
                floorItems[i].gridX == player.getGridX() &&
                floorItems[i].gridY == player.getGridY()) {
              bool isScroll = floorItems[i].type == ITEM_FIREBALL_SCROLL ||
                              floorItems[i].type == ITEM_FREEZE_SCROLL   ||
                              floorItems[i].type == ITEM_TELEPORT_SCROLL;
              int charges = (player.IsMage() && isScroll) ? 2 : 1;
              if (player.inventory.addItem(floorItems[i].type, charges)) {
                floorItems[i].pickedUp = true;
                combatLog.add("Picked up " + getItemName(floorItems[i].type));
                PlaySound(sndPickUp);
              }
            }
          }

          // Player turn
          int playerResult = player.movement(gameMap, enemies);

          if (playerResult != 0) {
            if (playerResult > 0) {
              combatLog.add("You hit for " + std::to_string(playerResult) + " damage");
              PlaySound(sndHitHurt);
            }

            pendingPlayerActions--;

            if (pendingPlayerActions <= 0) {
              // Enemies take their turn
              for (int i = 0; i < (int)enemies.size(); i++) {
                if (enemies[i].isAlive()) {
                  int dmg = enemies[i].takeTurn(
                    player.getGridX(), player.getGridY(), gameMap, enemies);
                  if (dmg > 0) {
                    bool dodged = player.TakeDamage(dmg);
                    if (dodged) {
                      combatLog.add("You dodged the attack!");
                      continue;
                    }
                    combatLog.add("Enemy hits you for " + std::to_string(dmg));
                    PlaySound(sndTakeDmg);
                    if (!player.isAlive()) {
                      deathFloor    = currentFloor;
                      gameState     = GAME_OVER;
                      gameOverAlpha = 0.0f;
                      PlaySound(sndDeath);
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

              // Roll actions for next round
              pendingPlayerActions = 1;
              float bonus = player.GetSpeedBonusChance();
              if (bonus > 0.0f && GetRandomValue(0, 99) < (int)(bonus * 100))
                pendingPlayerActions++;
            }
          }
        }

        } // end else (not pendingModifierSelect)

      } else {
        // Transition is active — swap map when ready
        if (transition.ShouldSwapMap()) {
          currentFloor++;
          isTargeting = false;
          combatLog.clear();

          if (currentFloor > 10) {
            // Enter the win room
            gameMap.generateWinRoom();
            player.setGridPosition(WIN_PLAYER_START_X, WIN_PLAYER_START_Y);
            enemies.clear();
            floorItems.clear();
            winChestPickedUp = false;
            gameState = WIN_ROOM;
          } else {
            combatLog.add("You descend to floor " + std::to_string(currentFloor));
            gameMap.generate();
            gameMap.forceFloor(player.getGridX(), player.getGridY());
            reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
            gameMap.spawnStairsInRegion(reachable, player.getGridX(), player.getGridY());
            spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
            spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
            offeredModifiers      = generateModifierOffers();
            modifierSelectIndex   = 0;
            pendingModifierSelect = true;
            pendingPlayerActions  = 1;
          }
        }
      }
    } else if (gameState == MODIFIER_SELECT) {
      if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A))
        modifierSelectIndex = (modifierSelectIndex - 1 + 3) % 3;
      if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        modifierSelectIndex = (modifierSelectIndex + 1) % 3;
      if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        player.ApplyModifier(offeredModifiers[modifierSelectIndex]);
        combatLog.add(std::string("Gained: ") +
                      GetModifierTypeName(offeredModifiers[modifierSelectIndex].type) +
                      " " + GetModifierTierName(offeredModifiers[modifierSelectIndex].tier));
        gameState = PLAYING;
      }
    } else if (gameState == WIN_ROOM) {
      if (!winChestPickedUp) {
        std::vector<Enemy> noEnemies;
        player.movement(gameMap, noEnemies);

        if (player.getGridX() == WIN_CHEST_X && player.getGridY() == WIN_CHEST_Y) {
          winChestPickedUp = true;
        }
      } else {
        if (IsKeyPressed(KEY_ENTER)) {
          // Reset and return to main menu
          currentFloor         = 1;
          isTargeting          = false;
          winChestPickedUp     = false;
          combatLog.clear();
          gameMap.generate();
          player = Player();
          player.spawn(gameMap);
          reachable = gameMap.getReachableFloorPositions(player.getGridX(), player.getGridY());
          gameMap.spawnStairsInRegion(reachable, player.getGridX(), player.getGridY());
          spawnEnemies(enemies, gameMap, currentFloor, player.getGridX(), player.getGridY(), reachable);
          spawnItems(floorItems, gameMap, player.getGridX(), player.getGridY(), reachable);
          gameState = MENU;
        }
      }
    }
    // --- 2. DRAW PHASE (The Eyes) ---
    BeginDrawing();
    ClearBackground(BLACK);

    if (gameState == MENU) {
      mainMenu.Draw();
    } else if (gameState == CHARACTER_SELECT) {
      const char* title = "Choose Your Hero";
      int titleW = (int)MeasureTextEx(gFont, title, 24, 1.0f).x;
      DrawTextEx(gFont, title, {(float)(screenWidth / 2 - titleW / 2), 100.0f}, 24, 1.0f, WHITE);

      struct HeroCard { const char* name; const char* subtitle; Color color; };
      HeroCard heroes[3] = {
        { "Warrior", "More Health",              Color{180,  60,  60, 255} },
        { "Rogue",   "Quick in Battle",          Color{ 60, 180,  90, 255} },
        { "Mage",    "Weak, but uses Scrolls\nTwice", Color{ 80,  80, 200, 255} },
      };

      int cardW = 240, cardH = 200;
      int totalW = cardW * 3 + 40 * 2;
      int startX = screenWidth / 2 - totalW / 2;
      int cardY  = screenHeight / 2 - cardH / 2;

      for (int i = 0; i < 3; i++) {
        int cx = startX + i * (cardW + 40);
        bool sel = (i == characterSelectIndex);
        Color border = sel ? YELLOW : DARKGRAY;
        Color bg     = sel ? Color{30, 30, 50, 255} : Color{15, 15, 25, 255};

        DrawRectangle(cx, cardY, cardW, cardH, bg);
        DrawRectangleLines(cx, cardY, cardW, cardH, border);

        // Class colour swatch
        DrawRectangle(cx + cardW / 2 - 20, cardY + 20, 40, 40, heroes[i].color);

        // Name
        int nameW = (int)MeasureTextEx(gFont, heroes[i].name, 16, 1.0f).x;
        DrawTextEx(gFont, heroes[i].name,
          {(float)(cx + cardW / 2 - nameW / 2), (float)(cardY + 75)}, 16, 1.0f,
          sel ? YELLOW : WHITE);

        // Subtitle — handle manual newline
        std::string sub = heroes[i].subtitle;
        int nl = (int)sub.find('\n');
        if (nl == (int)std::string::npos) {
          int subW = (int)MeasureTextEx(gFont, sub.c_str(), 10, 1.0f).x;
          DrawTextEx(gFont, sub.c_str(),
            {(float)(cx + cardW / 2 - subW / 2), (float)(cardY + 110)}, 10, 1.0f, LIGHTGRAY);
        } else {
          std::string line1 = sub.substr(0, nl);
          std::string line2 = sub.substr(nl + 1);
          int w1 = (int)MeasureTextEx(gFont, line1.c_str(), 10, 1.0f).x;
          int w2 = (int)MeasureTextEx(gFont, line2.c_str(), 10, 1.0f).x;
          DrawTextEx(gFont, line1.c_str(),
            {(float)(cx + cardW / 2 - w1 / 2), (float)(cardY + 110)}, 10, 1.0f, LIGHTGRAY);
          DrawTextEx(gFont, line2.c_str(),
            {(float)(cx + cardW / 2 - w2 / 2), (float)(cardY + 128)}, 10, 1.0f, LIGHTGRAY);
        }
      }

      const char* hint = "Left/Right to select   Enter to confirm";
      int hintW = (int)MeasureTextEx(gFont, hint, 10, 1.0f).x;
      DrawTextEx(gFont, hint,
        {(float)(screenWidth / 2 - hintW / 2), (float)(cardY + cardH + 30)}, 10, 1.0f, GRAY);
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
    } else if (gameState == PLAYING) {
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
    } else if (gameState == MODIFIER_SELECT) {
      // Dim background
      DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 200});

      const char* title = "Choose a Modifier";
      int titleW = (int)MeasureTextEx(gFont, title, 24, 1.0f).x;
      DrawTextEx(gFont, title, {(float)(screenWidth / 2 - titleW / 2), 120.0f}, 24, 1.0f, WHITE);

      // Draw 3 cards
      int cardW = 220, cardH = 160;
      int totalW = cardW * 3 + 40 * 2; // cards + gaps
      int startX = screenWidth / 2 - totalW / 2;
      int cardY  = screenHeight / 2 - cardH / 2;

      for (int i = 0; i < 3; i++) {
        int cx = startX + i * (cardW + 40);
        bool selected = (i == modifierSelectIndex);
        Color border = selected ? YELLOW : DARKGRAY;
        Color bg     = selected ? Color{40, 40, 60, 255} : Color{20, 20, 30, 255};

        DrawRectangle(cx, cardY, cardW, cardH, bg);
        DrawRectangleLines(cx, cardY, cardW, cardH, border);

        const char* typeName = GetModifierTypeName(offeredModifiers[i].type);
        const char* tierName = GetModifierTierName(offeredModifiers[i].tier);

        int typeW = (int)MeasureTextEx(gFont, typeName, 16, 1.0f).x;
        DrawTextEx(gFont, typeName,
          {(float)(cx + cardW / 2 - typeW / 2), (float)(cardY + 30)}, 16, 1.0f,
          selected ? YELLOW : WHITE);

        int tierW = (int)MeasureTextEx(gFont, tierName, 12, 1.0f).x;
        DrawTextEx(gFont, tierName,
          {(float)(cx + cardW / 2 - tierW / 2), (float)(cardY + 70)}, 12, 1.0f, GRAY);

        // Show bonus value
        float val = GetModifierValue(offeredModifiers[i].type, offeredModifiers[i].tier);
        std::string bonusStr;
        if (offeredModifiers[i].type == ModifierType::Speed)
            bonusStr = "+" + std::to_string(val).substr(0, std::to_string(val).find('.') + 2) + " spd";
        else if (offeredModifiers[i].type == ModifierType::Health)
            bonusStr = "+" + std::to_string((int)val) + " HP";
        else
            bonusStr = "+" + std::to_string((int)(val * 100)) + "%";
        int bonusW = (int)MeasureTextEx(gFont, bonusStr.c_str(), 14, 1.0f).x;
        DrawTextEx(gFont, bonusStr.c_str(),
          {(float)(cx + cardW / 2 - bonusW / 2), (float)(cardY + 100)}, 14, 1.0f, GREEN);
      }

      const char* hint = "Left/Right to select   Enter to confirm";
      int hintW = (int)MeasureTextEx(gFont, hint, 10, 1.0f).x;
      DrawTextEx(gFont, hint,
        {(float)(screenWidth / 2 - hintW / 2), (float)(cardY + cardH + 30)}, 10, 1.0f, GRAY);
    } else if (gameState == WIN_ROOM) {
      gameMap.drawMap();

      // Draw treasure chest if not yet picked up
      if (!winChestPickedUp) {
        drawFloorItem(ITEM_TREASURE_CHEST, WIN_CHEST_X, WIN_CHEST_Y);
      }

      player.drawPlayer();

      // "You Made It!" banner at top
      const char* banner = "You Made It!";
      int bannerW = (int)MeasureTextEx(gFont, banner, 28, 1.0f).x;
      DrawTextEx(gFont, banner,
        {(float)(MAP_AREA_W / 2 - bannerW / 2), 20.0f}, 28, 1.0f, GOLD);

      if (!winChestPickedUp) {
        const char* hint = "Find the treasure chest...";
        int hintW = (int)MeasureTextEx(gFont, hint, 10, 1.0f).x;
        DrawTextEx(gFont, hint,
          {(float)(MAP_AREA_W / 2 - hintW / 2), (float)(screenHeight - 30)}, 10, 1.0f, GRAY);
      } else {
        // Win overlay
        DrawRectangle(0, 0, MAP_AREA_W, screenHeight, Color{0, 0, 0, 180});

        const char* wonStr = "YOU WON!";
        int wonW = (int)MeasureTextEx(gFont, wonStr, 56, 1.0f).x;
        DrawTextEx(gFont, wonStr,
          {(float)(MAP_AREA_W / 2 - wonW / 2), (float)(screenHeight / 2 - 80)},
          56, 1.0f, GOLD);

        const char* subStr = "The dungeon has been conquered.";
        int subW = (int)MeasureTextEx(gFont, subStr, 14, 1.0f).x;
        DrawTextEx(gFont, subStr,
          {(float)(MAP_AREA_W / 2 - subW / 2), (float)(screenHeight / 2 + 10)},
          14, 1.0f, WHITE);

        const char* enterStr = "Press ENTER to return to menu";
        int enterW = (int)MeasureTextEx(gFont, enterStr, 12, 1.0f).x;
        DrawTextEx(gFont, enterStr,
          {(float)(MAP_AREA_W / 2 - enterW / 2), (float)(screenHeight / 2 + 50)},
          12, 1.0f, GRAY);
      }
    }

    EndDrawing();
  }

  UnloadSound(sndHitHurt);
  UnloadSound(sndTakeDmg);
  UnloadSound(sndSpell);
  UnloadSound(sndPickUp);
  UnloadSound(sndDescend);
  UnloadSound(sndDeath);
  CloseAudioDevice();
  UnloadFont(gFont);
  CloseWindow();
  return 0;
}

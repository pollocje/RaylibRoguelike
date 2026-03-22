#include "Map.h"
#include "Modifiers.h"
#include "Player.h"

#include "raylib.h"

const int screenWidth = 1280;
const int screenHeight = 720;

int main() {

  InitWindow(screenWidth, screenHeight, "semGame");
  SetTargetFPS(60);

  // Create Player on Start
  Player player;

  // Create Map
  Map map(screenWidth, screenHeight);

  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_SPACE)) {
      map.generate();
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

    map.drawMap();
    player.drawPlayer();
    player.DrawHUD();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

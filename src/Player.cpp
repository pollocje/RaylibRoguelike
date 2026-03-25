#include "Player.h"
#include "raylib.h"
#include "Map.h"
#include <iostream>
#include <string>

// Constructor
Player::Player() {
    x = 400.0f;
    y = 225.0f;
    radius = 10.0f;

    // Base stats
    baseHealth = 100;
    mana = 100;
    carryWeight = 0;
    baseSpeed = 4.0f;
    baseFireRate = 1.0f;
    baseLuck = 0.0f;
    baseDodgeChance = 0.0f;
    baseXPGainMultiplier = 1.0f;

    xp = 0;
    reputation = 0;

    ResetStatsToBase();
}

void Player::ResetStatsToBase() {
    maxHealth = baseHealth;
    currentHealth = maxHealth;
    speed = baseSpeed;
    fireRate = baseFireRate;
    luckChance = baseLuck;
    dodgeChance = baseDodgeChance;
    xpGainMultiplier = baseXPGainMultiplier;
}

void Player::drawPlayer() const {
    DrawCircle((int)x, (int)y, radius, BLUE);
}

void Player::DrawHUD() const {
    DrawText(TextFormat("Health: %d / %d", currentHealth, maxHealth), 20, 20, 20, WHITE);
    DrawText(TextFormat("Speed: %.2f", speed), 20, 50, 20, WHITE);
    DrawText(TextFormat("Fire Rate: %.2f", fireRate), 20, 80, 20, WHITE);
    DrawText(TextFormat("Luck: %.0f%%", luckChance * 100.0f), 20, 110, 20, WHITE);
    DrawText(TextFormat("Dodge: %.0f%%", dodgeChance * 100.0f), 20, 140, 20, WHITE);
    DrawText(TextFormat("XP Gain: %.0f%%", (xpGainMultiplier - 1.0f) * 100.0f), 20, 170, 20, WHITE);
    DrawText(TextFormat("XP: %d", xp), 20, 200, 20, WHITE);
    DrawText(TextFormat("Reputation: %d", reputation), 20, 230, 20, WHITE);
}

void Player::movement(const Map& map) {
    float nextX = x;
    float nextY = y;

    if (IsKeyDown(KEY_W)) y -= speed;
    if (IsKeyDown(KEY_S)) y += speed;
    if (IsKeyDown(KEY_A)) x -= speed;
    if (IsKeyDown(KEY_D)) x += speed;

    int targetCellX = (int)(nextX / map.getCellW());
    int targetCellY = (int)(nextY / map.getCellH());

    if (map.isCellWalkable(targetCellX, targetCellY)) {
        x = nextX;
        y = nextY;
    }
}

void Player::ApplyModifier(const Modifier& modifier) {
    float value = GetModifierValue(modifier.type, modifier.tier);

    switch (modifier.type) {
    case ModifierType::Speed:
        speed = baseSpeed * (1.0f + value);
        break;

    case ModifierType::Health:
        maxHealth = (int)(baseHealth * (1.0f + value));
        if (currentHealth > maxHealth) {
            currentHealth = maxHealth;
        }
        break;

    case ModifierType::FireRate:
        fireRate = baseFireRate * (1.0f + value);
        break;

    case ModifierType::Luck:
        luckChance = value;
        break;

    case ModifierType::Dodge:
        dodgeChance = value;
        break;

    case ModifierType::XPGain:
        xpGainMultiplier = 1.0f + value;
        break;
    }
}

void Player::TakeDamage(int amount) {
    if (TryDodge()) {
        std::cout << "Attack dodged!" << std::endl;
        return;
    }

    currentHealth -= amount;

    if (currentHealth < 0) {
        currentHealth = 0;
    }
}

void Player::Heal(int amount) {
    currentHealth += amount;

    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }
}

void Player::GainXP(int baseXP) {
    int rewardedXP = GetXPReward(baseXP);
    xp += rewardedXP;

    // simple placeholder reputation gain
    reputation += rewardedXP / 10;
}

int Player::GetDamageOutput(int baseDamage) const {
    if (TryDoubleDamage()) {
        return baseDamage * 2;
    }

    return baseDamage;
}

bool Player::TryDodge() const {
    float roll = GetRandomValue(0, 10000) / 10000.0f;
    return roll < dodgeChance;
}

bool Player::TryDoubleDamage() const {
    float roll = GetRandomValue(0, 10000) / 10000.0f;
    return roll < luckChance;
}

int Player::GetXPReward(int baseXP) const {
    return (int)(baseXP * xpGainMultiplier);
}

int Player::GetCurrentHealth() const {
    return currentHealth;
}

int Player::GetMaxHealth() const {
    return maxHealth;
}

float Player::GetSpeed() const {
    return speed;
}

float Player::GetFireRate() const {
    return fireRate;
}

int Player::GetXP() const {
    return xp;
}

void Player::drawPlayer() {
  int pixelX = (gridX * 40) + 20;
  int pixelY = (gridY * 40) + 20;

  DrawCircle(pixelX , pixelY, 15, BLUE);
}

// MAIN MOVEMENT METHOD
// Player needs to see the map
void Player::movement(Map &mapData) {

  // move up
  if (IsKeyPressed(KEY_UP)) {
    if (mapData.isCellWalkable(gridX, gridY - 1)) {
      gridY -= 1;
    }
  }

  // move down
  if (IsKeyPressed(KEY_DOWN)) {
    if (mapData.isCellWalkable(gridX, gridY + 1)) {
      gridY += 1;
    }
  }

  // move right
  if (IsKeyPressed(KEY_RIGHT)) {
    if (mapData.isCellWalkable(gridX + 1, gridY)) {
      gridX += 1;
    }
  }

  // move left
  if (IsKeyPressed(KEY_LEFT)) {
    if (mapData.isCellWalkable(gridX - 1, gridY)) {
      gridX -= 1;
    }
  }
}

void Player::spawn(Map &mapData) {
  Vector2 gridPos = mapData.getRandomFloorGridPos();
  gridX = (int)gridPos.x;
  gridY = (int)gridPos.y;
}

void Player::setGridPosition(int x, int y) {
  gridX = x;
  gridY = y;
void Player::PrintStats() const {
    std::cout << "----- Player Stats -----" << std::endl;
    std::cout << "Max Health: " << maxHealth << std::endl;
    std::cout << "Current Health: " << currentHealth << std::endl;
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Fire Rate: " << fireRate << std::endl;
    std::cout << "Luck Chance: " << luckChance << std::endl;
    std::cout << "Dodge Chance: " << dodgeChance << std::endl;
    std::cout << "XP Gain Multiplier: " << xpGainMultiplier << std::endl;
    std::cout << "XP: " << xp << std::endl;
    std::cout << "Reputation: " << reputation << std::endl;
}

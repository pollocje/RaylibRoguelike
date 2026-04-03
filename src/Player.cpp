#include "Player.h"
#include "Enemy.h"
#include "Map.h"
#include "raylib.h"
#include <iostream>
#include <cstdlib>

Player::Player() {
    gridX = 0;
    gridY = 0;

    baseHealth           = 100;
    baseSpeed            = 4.0f;
    baseLuck             = 0.0f;
    baseDodgeChance      = 0.0f;
    baseXPGainMultiplier = 1.0f;

    attack               = 10;
    rageTurnsRemaining   = 0;
    isMage               = false;
    classColor           = BLUE;
    classShape           = 0;
    xp                   = 0;
    reputation = 0;
    level      = 0;

    currentHealth = baseHealth;
    RecalculateStats();
}

// Recomputes all current stats from base values + accumulated modifiers.
// Each modifier type stacks additively, allowing multiple picks of the same type.
void Player::RecalculateStats() {
    maxHealth        = baseHealth;
    speed            = baseSpeed;
    luckChance       = baseLuck;
    dodgeChance      = baseDodgeChance;
    xpGainMultiplier = baseXPGainMultiplier;

    for (const Modifier& mod : activeModifiers) {
        float value = GetModifierValue(mod.type, mod.tier);
        switch (mod.type) {
        case ModifierType::Speed:
            speed += baseSpeed * value;
            break;
        case ModifierType::Health:
            maxHealth += (int)(baseHealth * value);
            break;
        case ModifierType::Luck:
            luckChance += value;
            break;
        case ModifierType::Dodge:
            dodgeChance += value;
            break;
        case ModifierType::XPGain:
            xpGainMultiplier += value;
            break;
        }
    }

    // Cap probabilities at 95%
    if (luckChance  > 0.95f) luckChance  = 0.95f;
    if (dodgeChance > 0.95f) dodgeChance = 0.95f;

    if (currentHealth > maxHealth) currentHealth = maxHealth;
}

void Player::ResetStatsToBase() {
    activeModifiers.clear();
    currentHealth = baseHealth;
    RecalculateStats();
}

void Player::IncrementLevel() {
    level++;
}

void Player::drawPlayer() {
    int px = gridX * 40 + 20;
    int py = gridY * 40 + 20;
    int half = 12;
    DrawRectangle(px - half, py - half, half * 2, half * 2, classColor);
}

void Player::DrawHUD() const {
    DrawText(TextFormat("Level: %d", level), 20, 20, 20, YELLOW);
    DrawText(TextFormat("Health: %d / %d", currentHealth, maxHealth), 20, 50, 20, WHITE);
    DrawText(TextFormat("Speed: %.2f", speed), 20, 80, 20, WHITE);
    DrawText(TextFormat("Luck: %.0f%%", luckChance * 100.0f), 20, 110, 20, WHITE);
    DrawText(TextFormat("Dodge: %.0f%%", dodgeChance * 100.0f), 20, 170, 20, WHITE);
    DrawText(TextFormat("XP Gain: +%.0f%%", (xpGainMultiplier - 1.0f) * 100.0f), 20, 200, 20, WHITE);
    DrawText(TextFormat("XP: %d", xp), 20, 230, 20, WHITE);
}

// Returns 0 = no action, -1 = moved/bumped wall, >0 = damage dealt to enemy
int Player::movement(Map& mapData, std::vector<Enemy>& enemies) {
    int newX = gridX;
    int newY = gridY;
    bool keyPressed = false;

    if      (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) { newY -= 1; keyPressed = true; }
    else if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) { newY += 1; keyPressed = true; }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { newX += 1; keyPressed = true; }
    else if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) { newX -= 1; keyPressed = true; }

    if (!keyPressed) return 0;

    bool raging = rageTurnsRemaining > 0;
    if (raging) rageTurnsRemaining--;

    // Bump attack: check if an enemy occupies the target tile
    for (int i = 0; i < (int)enemies.size(); i++) {
        if (enemies[i].isAlive() && enemies[i].gridX == newX && enemies[i].gridY == newY) {
            int dmg = (rand() % attack) + 1;
            if (raging) dmg *= 3;
            enemies[i].takeDamage(dmg);
            return dmg;
        }
    }

    // Normal movement — wall bumps still count as a turn
    if (mapData.isCellWalkable(newX, newY)) {
        gridX = newX;
        gridY = newY;
    }

    return -1;
}

void Player::ApplyRage(int turns) {
    rageTurnsRemaining += turns;
}

void Player::ApplyCharacterClass(float hpMult, float atkMult, float dodgeMult, float spdMult, bool mage) {
    const float refDodge = 0.10f;

    baseHealth      = (int)(baseHealth * hpMult);
    attack          = (int)(attack     * atkMult);
    baseDodgeChance = refDodge * dodgeMult;
    baseSpeed       = baseSpeed * spdMult;
    isMage          = mage;

    // Set class visuals
    if (!mage && atkMult == 1.0f) {           // Warrior
        classColor = Color{180,  60,  60, 255};
        classShape = 0;
    } else if (!mage && atkMult > 1.0f) {     // Rogue
        classColor = Color{ 60, 180,  90, 255};
        classShape = 1;
    } else {                                   // Mage
        classColor = Color{ 80,  80, 200, 255};
        classShape = 2;
    }

    currentHealth = baseHealth;
    RecalculateStats();
}

void Player::spawn(Map& mapData) {
    Vector2 gridPos = mapData.getRandomFloorGridPos();
    gridX = (int)gridPos.x;
    gridY = (int)gridPos.y;
}

void Player::setGridPosition(int x, int y) {
    gridX = x;
    gridY = y;
}

void Player::ApplyModifier(const Modifier& modifier) {
    activeModifiers.push_back(modifier);
    RecalculateStats();
}

void Player::applyDamage(int amount) {
    currentHealth -= amount;
    if (currentHealth < 0) currentHealth = 0;
}

void Player::heal(int amount) {
    currentHealth += amount;
    if (currentHealth > maxHealth) currentHealth = maxHealth;
}

bool Player::TakeDamage(int amount) {
    if (TryDodge()) return true;
    applyDamage(amount);
    return false;
}

void Player::Heal(int amount) {
    heal(amount);
}

void Player::GainXP(int baseXP) {
    int rewardedXP = GetXPReward(baseXP);
    xp += rewardedXP;
    reputation += rewardedXP / 10;
}

int Player::GetDamageOutput(int baseDamage) const {
    if (TryDoubleDamage()) return baseDamage * 2;
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

bool Player::isAlive() const {
    return currentHealth > 0;
}

float Player::GetSpeed() const    { return speed; }

float Player::GetSpeedBonusChance() const {
    return (speed - baseSpeed) / baseSpeed;
}

int   Player::GetXP() const       { return xp; }
int   Player::GetReputation() const { return reputation; }

void Player::PrintStats() const {
    std::cout << "----- Player Stats (Level " << level << ") -----" << std::endl;
    std::cout << "Max Health:    " << maxHealth       << std::endl;
    std::cout << "Curr Health:   " << currentHealth   << std::endl;
    std::cout << "Speed:         " << speed           << std::endl;
    std::cout << "Luck Chance:   " << luckChance      << std::endl;
    std::cout << "Dodge Chance:  " << dodgeChance     << std::endl;
    std::cout << "XP Gain Multi: " << xpGainMultiplier << std::endl;
    std::cout << "XP:            " << xp              << std::endl;
    std::cout << "Reputation:    " << reputation      << std::endl;
}

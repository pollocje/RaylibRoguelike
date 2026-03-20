#include "Player.h"
#include "raylib.h"
#include <iostream>

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

void Player::drawPlayer() {
    DrawCircle((int)x, (int)y, radius, BLUE);
}

void Player::movement() {
    if (IsKeyDown(KEY_W)) y -= speed;
    if (IsKeyDown(KEY_S)) y += speed;
    if (IsKeyDown(KEY_A)) x -= speed;
    if (IsKeyDown(KEY_D)) x += speed;
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

void Player::PrintStats() const {
    std::cout << "----- Player Stats -----" << std::endl;
    std::cout << "Max Health: " << maxHealth << std::endl;
    std::cout << "Current Health: " << currentHealth << std::endl;
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Fire Rate: " << fireRate << std::endl;
    std::cout << "Luck Chance: " << luckChance << std::endl;
    std::cout << "Dodge Chance: " << dodgeChance << std::endl;
    std::cout << "XP Gain Multiplier: " << xpGainMultiplier << std::endl;
}

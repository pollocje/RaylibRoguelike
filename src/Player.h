#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "Modifiers.h"

class Player {
private:
    // Position
    float x;
    float y;
    float radius;

    // Base Stats
    int baseHealth;
    int mana;
    int carryWeight;
    float baseSpeed;
    float baseFireRate;
    float baseLuck;
    float baseDodgeChance;
    float baseXPGainMultiplier;

    // Current/Modified Stats
    int maxHealth;
    int currentHealth;
    float speed;
    float fireRate;
    float luckChance;
    float dodgeChance;
    float xpGainMultiplier;

public:
    // Constructor
    Player();

    void drawPlayer();
    void movement();

    // Modifier methods
    void ApplyModifier(const Modifier& modifier);
    void ResetStatsToBase();

    // Utility/gameplay methods
    bool TryDodge() const;
    bool TryDoubleDamage() const;
    int GetXPReward(int baseXP) const;

    // Optional debug helper
    void PrintStats() const;
};

#endif

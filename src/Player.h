#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "Modifiers.h"

class Map;

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

    // Progression
    int xp;
    int reputation;

public:
    // Constructor
    Player();

    void drawPlayer() const;
    void DrawHUD() const;

    void movement(const Map& map);

    // Modifier methods
    void ApplyModifier(const Modifier& modifier);
    void ResetStatsToBase();

    // Gameplay methods
    void TakeDamage(int amount);
    void Heal(int amount);
    void GainXP(int baseXP);
    int GetDamageOutput(int baseDamage) const;

    // Utility methods
    bool TryDodge() const;
    bool TryDoubleDamage() const;
    int GetXPReward(int baseXP) const;

    // Getters
    int GetCurrentHealth() const;
    int GetMaxHealth() const;
    float GetSpeed() const;
    float GetFireRate() const;
    int GetXP() const;
    int GetReputation() const;

    // Optional debug helper
    void PrintStats() const;
};

#endif

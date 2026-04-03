#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "Modifiers.h"
#include "Inventory.h"
#include <vector>

class Map;
class Enemy;

class Player {
private:
    // Grid position
    int gridX;
    int gridY;

    // Base Stats (anchors for modifier recalculation)
    int baseHealth;
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

    // Combat
    int attack;

    // Progression
    int xp;
    int reputation;
    int level;

    // Accumulated modifiers (stacks across levels)
    std::vector<Modifier> activeModifiers;

    void RecalculateStats();

public:
    Inventory inventory;

    Player();

    void drawPlayer();
    void DrawHUD() const;

    // Returns 0 = no action, -1 = moved/bumped wall, >0 = damage dealt to enemy
    int movement(Map& mapData, std::vector<Enemy>& enemies);
    void spawn(Map& mapData);
    void setGridPosition(int x, int y);

    // Modifier methods
    void ApplyModifier(const Modifier& modifier);
    void ResetStatsToBase();
    void IncrementLevel();

    // Gameplay methods
    void applyDamage(int amount);   // direct damage from enemies (no dodge roll)
    void heal(int amount);          // direct heal from items
    void TakeDamage(int amount);    // player-initiated dodge roll then damage
    void Heal(int amount);          // alias for heal
    void GainXP(int baseXP);
    int GetDamageOutput(int baseDamage) const;

    // Utility
    bool TryDodge() const;
    bool TryDoubleDamage() const;
    int GetXPReward(int baseXP) const;
    bool isAlive() const;

    // Getters (lowercase match existing call sites in main.cpp)
    int getGridX() const    { return gridX; }
    int getGridY() const    { return gridY; }
    int getHealth() const   { return currentHealth; }
    int getMaxHealth() const { return maxHealth; }
    // CamelCase versions for modifier/HUD code
    int GetCurrentHealth() const { return currentHealth; }
    int GetMaxHealth() const     { return maxHealth; }
    float GetSpeed() const;
    float GetFireRate() const;
    int GetXP() const;
    int GetReputation() const;
    int GetLevel() const { return level; }

    void PrintStats() const;
};

#endif

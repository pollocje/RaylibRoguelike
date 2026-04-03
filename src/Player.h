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
    float baseLuck;
    float baseDodgeChance;
    float baseXPGainMultiplier;

    // Current/Modified Stats
    int maxHealth;
    int currentHealth;
    float speed;
    float luckChance;
    float dodgeChance;
    float xpGainMultiplier;

    // Combat
    int attack;
    int rageTurnsRemaining;
    bool isMage;

    // Visual
    Color classColor;
    int   classShape; // 0 = circle (Warrior), 1 = triangle (Rogue), 2 = diamond (Mage)

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
    void ApplyCharacterClass(float hpMult, float atkMult, float dodgeMult, float spdMult, bool mage);
    void ResetStatsToBase();
    void IncrementLevel();

    // Gameplay methods
    void applyDamage(int amount);   // direct damage from enemies (no dodge roll)
    void heal(int amount);          // direct heal from items
    bool TakeDamage(int amount);    // dodge roll then damage; returns true if dodged
    void Heal(int amount);          // alias for heal
    void ApplyRage(int turns);     // rage lasts for N total turns
    bool IsRaging() const { return rageTurnsRemaining > 0; }
    bool IsMage()   const { return isMage; }
    int  GetRageTurnsRemaining() const { return rageTurnsRemaining; }
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
    float GetSpeedBonusChance() const;
    float GetDodgeChance() const     { return dodgeChance; }
    float GetLuckChance() const      { return luckChance; }
    float GetXPGainMultiplier() const { return xpGainMultiplier; }
    int GetXP() const;
    int GetReputation() const;
    int GetLevel() const { return level; }

    void PrintStats() const;
};

#endif

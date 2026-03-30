#ifndef ENEMY_H
#define ENEMY_H

#include "Map.h"
#include "raylib.h"
#include <vector>

class Enemy {
public:
    int gridX, gridY;
    int health;
    int maxHealth;
    int attack;

    Enemy(int x, int y, int floor);

    void draw() const;
    void takeDamage(int amount);
    bool isAlive() const;

    // Move toward player or attack if adjacent. Returns damage dealt to player.
    int takeTurn(int playerX, int playerY, Map &mapData, std::vector<Enemy> &enemies);

private:
    bool canMoveTo(int tx, int ty, Map &mapData, std::vector<Enemy> &enemies);
};

#endif

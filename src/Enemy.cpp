#include "Enemy.h"
#include <cstdlib>

Enemy::Enemy(int x, int y, int floor)
    : gridX(x), gridY(y) {
    attack    = 2 + floor;
    maxHealth = 10;
    health    = maxHealth;
}

void Enemy::draw() const {
    int px = gridX * 40 + 20;
    int py = gridY * 40 + 20;
    DrawCircle(px, py, 14, RED);

    // Health bar above enemy
    int barW = 30;
    int filled = (health * barW) / maxHealth;
    DrawRectangle(px - 15, py - 22, barW, 4, DARKGRAY);
    DrawRectangle(px - 15, py - 22, filled, 4, GREEN);
}

void Enemy::takeDamage(int amount) {
    health -= amount;
}

bool Enemy::isAlive() const {
    return health > 0;
}

// Helper: check if a tile is walkable and not occupied by another enemy
bool Enemy::canMoveTo(int tx, int ty, Map &mapData, std::vector<Enemy> &enemies) {
    if (!mapData.isCellWalkable(tx, ty)) return false;
    for (int i = 0; i < (int)enemies.size(); i++) {
        if (&enemies[i] != this && enemies[i].gridX == tx && enemies[i].gridY == ty)
            return false;
    }
    return true;
}

int Enemy::takeTurn(int playerX, int playerY, Map &mapData, std::vector<Enemy> &enemies) {
    int dx = playerX - gridX;
    int dy = playerY - gridY;
    int absDx = (dx < 0 ? -dx : dx);
    int absDy = (dy < 0 ? -dy : dy);

    // Adjacent to player: attack instead of moving
    if (absDx + absDy == 1) {
        return (rand() % attack) + 1;
    }

    int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
    int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

    // Prefer moving along the larger axis, fall back to the other
    if (absDx >= absDy) {
        if (stepX != 0 && canMoveTo(gridX + stepX, gridY, mapData, enemies))
            gridX += stepX;
        else if (stepY != 0 && canMoveTo(gridX, gridY + stepY, mapData, enemies))
            gridY += stepY;
    } else {
        if (stepY != 0 && canMoveTo(gridX, gridY + stepY, mapData, enemies))
            gridY += stepY;
        else if (stepX != 0 && canMoveTo(gridX + stepX, gridY, mapData, enemies))
            gridX += stepX;
    }

    return 0;
}

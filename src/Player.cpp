#include "Player.h"
#include "Enemy.h"
#include "Map.h"
#include "raylib.h"
#include <cstdlib>

Player::Player() {
  health    = 100;
  maxHealth = 100;
  mana      = 100;
  carryWeight = 0;
  attack    = 10;

  gridX = 4;
  gridY = 4;
}

void Player::drawPlayer() {
  int pixelX = (gridX * 40) + 20;
  int pixelY = (gridY * 40) + 20;

  DrawCircle(pixelX, pixelY, 15, BLUE);
}

void Player::applyDamage(int amount) {
  health -= amount;
  if (health < 0) health = 0;
}

// Returns true if the player took an action (so enemies get their turn)
bool Player::movement(Map &mapData, std::vector<Enemy> &enemies) {
  int newX = gridX;
  int newY = gridY;
  bool keyPressed = false;

  if (IsKeyPressed(KEY_UP))    { newY -= 1; keyPressed = true; }
  else if (IsKeyPressed(KEY_DOWN))  { newY += 1; keyPressed = true; }
  else if (IsKeyPressed(KEY_RIGHT)) { newX += 1; keyPressed = true; }
  else if (IsKeyPressed(KEY_LEFT))  { newX -= 1; keyPressed = true; }

  if (!keyPressed) return false;

  // Check if an enemy is on the target tile (bump attack)
  for (int i = 0; i < (int)enemies.size(); i++) {
    if (enemies[i].isAlive() && enemies[i].gridX == newX && enemies[i].gridY == newY) {
      int dmg = (rand() % attack) + 1;
      enemies[i].takeDamage(dmg);
      return true; // turn taken (attack), player does not move
    }
  }

  // Normal movement — wall bumps still count as a turn
  if (mapData.isCellWalkable(newX, newY)) {
    gridX = newX;
    gridY = newY;
  }

  return true;
}

void Player::spawn(Map &mapData) {
  Vector2 gridPos = mapData.getRandomFloorGridPos();
  gridX = (int)gridPos.x;
  gridY = (int)gridPos.y;
}

void Player::setGridPosition(int x, int y) {
  gridX = x;
  gridY = y;
}

#include "Player.h"
#include "MAP.h"
#include "raylib.h"

// Implement Constructor
Player::Player() {
  health = 100;
  mana = 100;
  carryWeight = 0;

  gridX = 4;
  gridY = 4;
};

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
}

#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"

class Player {
private:
  // Basic Stats
  int health;
  int mana;
  int carryWeight;

  // coordinates
  int gridX;
  int gridY;

public:
  // Constructor
  Player();
  void drawPlayer();
  void movement(Map &mapData);
  void spawn(Map &mapData);
  void setGridPosition(int x, int y);

  // Getter functions for coordinates
  int getGridX() { return gridX; }
  int getGridY() { return gridY; }
};

#endif

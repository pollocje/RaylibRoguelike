#ifndef PLAYER_H
#define PLAYER_H

#include "MAP.h"

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
};

#endif

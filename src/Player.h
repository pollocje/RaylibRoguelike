#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"
#include "Inventory.h"
#include <vector>

class Enemy; // forward declaration — avoids circular include

class Player {
private:
  // Basic Stats
  int health;
  int maxHealth;
  int mana;
  int carryWeight;
  int attack;

  // coordinates
  int gridX;
  int gridY;

public:
  Inventory inventory;

  // Constructor
  Player();
  void drawPlayer();
  void heal(int amount);
  // Returns 0 = no action, -1 = moved/bumped, >0 = damage dealt to enemy
  int movement(Map &mapData, std::vector<Enemy> &enemies);
  void spawn(Map &mapData);
  void setGridPosition(int x, int y);
  void applyDamage(int amount);

  // Getters
  int getGridX()    { return gridX; }
  int getGridY()    { return gridY; }
  int getHealth()   { return health; }
  int getMaxHealth(){ return maxHealth; }
  bool isAlive()    { return health > 0; }
};

#endif

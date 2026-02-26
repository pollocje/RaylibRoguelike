#include "Player.h"

// Implement Constructor
Player::Player() {
  health = 100;
  mana = 100;
  carryWeight = 0;
};

void Player::drawPlayer() { DrawCircle(400, 400, 10, BLUE); }

void Player::movement() {
  // Main movement function
  // Turn based, only use
  // IsKeyPressed()
}

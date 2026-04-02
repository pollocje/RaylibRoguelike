#include "MainMenu.h"
#include "GameFont.h"
#include "raylib.h"

MainMenu::MainMenu() { selectedButton = 1; }

// Changing Game State
MainMenu::MainMenuButtons MainMenu::Update() {

  // we can increment or deincrement the selectedbutton
  if (IsKeyPressed(KEY_DOWN)) {
    selectedButton++;
  }
  if (IsKeyPressed(KEY_UP)) {
    selectedButton--;
  }

  // WrapAround Mechanics
  if (selectedButton < 1) {
    selectedButton = 3;
  }
  if (selectedButton > 3) {
    selectedButton = 1;
  }

  // then choose the option depending on the selectedButton
  if (IsKeyPressed(KEY_ENTER)) {
    if (selectedButton == 1)
      return START;
    if (selectedButton == 2)
      return OPTIONS;
    if (selectedButton == 3)
      return EXIT;
  }

  return NONE; // this happens if enter is not selected
}

void MainMenu::Draw() {

  // create color coded variables to switch depdendant on selectedButton #
  Color startColor = (selectedButton == 1) ? RED : WHITE;
  Color optColor = (selectedButton == 2) ? RED : WHITE;
  Color exitColor = (selectedButton == 3) ? RED : WHITE;

  // Title
  int titleW = (int)MeasureTextEx(gFont, "Rogue!", 32, 1.0f).x;
  DrawTextEx(gFont, "Rogue!", {(float)(640 - titleW / 2), 100.0f}, 32, 1.0f, WHITE);

  // START
  int startW = (int)MeasureTextEx(gFont, "Start", 20, 1.0f).x;
  DrawTextEx(gFont, "Start", {(float)(640 - startW / 2), 200.0f}, 20, 1.0f, startColor);

  // OPTIONS
  int optW = (int)MeasureTextEx(gFont, "Options", 20, 1.0f).x;
  DrawTextEx(gFont, "Options", {(float)(640 - optW / 2), 270.0f}, 20, 1.0f, optColor);

  // EXIT
  int exitW = (int)MeasureTextEx(gFont, "Exit", 20, 1.0f).x;
  DrawTextEx(gFont, "Exit", {(float)(640 - exitW / 2), 340.0f}, 20, 1.0f, exitColor);
}

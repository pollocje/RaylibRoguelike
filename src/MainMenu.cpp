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
    selectedButton = 2;
  }
  if (selectedButton > 2) {
    selectedButton = 1;
  }

  // then choose the option depending on the selectedButton
  if (IsKeyPressed(KEY_ENTER)) {
    if (selectedButton == 1)
      return START;
    if (selectedButton == 2)
      return EXIT;
  }

  return NONE; // this happens if enter is not selected
}

void MainMenu::Draw() {

  // create color coded variables to switch depdendant on selectedButton #
  Color startColor = (selectedButton == 1) ? RED : WHITE;
  Color exitColor = (selectedButton == 2) ? RED : WHITE;

  // Title
  int titleW = (int)MeasureTextEx(gFont, "Raylib Roguelike", 32, 1.0f).x;
  DrawTextEx(gFont, "Raylib Roguelike", {(float)(640 - titleW / 2), 100.0f}, 32, 1.0f, WHITE);

  // START
  int startW = (int)MeasureTextEx(gFont, "Start", 20, 1.0f).x;
  DrawTextEx(gFont, "Start", {(float)(640 - startW / 2), 200.0f}, 20, 1.0f, startColor);

  // EXIT
  int exitW = (int)MeasureTextEx(gFont, "Exit", 20, 1.0f).x;
  DrawTextEx(gFont, "Exit", {(float)(640 - exitW / 2), 270.0f}, 20, 1.0f, exitColor);
}

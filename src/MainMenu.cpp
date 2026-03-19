#include "MainMenu.h"
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
  DrawText("Rogue!", 640, 100, 25, WHITE);

  // START
  DrawText("Start", 640, 200, 20, startColor);

  // OPTIONS
  DrawText("Options", 640, 300, 20, optColor);

  // EXIT
  DrawText("Exit", 640, 400, 20, exitColor);
}

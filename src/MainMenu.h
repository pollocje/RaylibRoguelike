#ifndef MAINMENU_H
#define MAINMENU_H

class MainMenu {
private:
  int selectedButton = 0;

public:
  enum MainMenuButtons { NONE, START, EXIT };

  // Constructor
  MainMenu();
  MainMenuButtons Update();
  void Draw();
};

#endif

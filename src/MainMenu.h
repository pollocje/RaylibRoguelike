class MainMenu {
private:
  int selectedButton = 0;

public:
  enum MainMenuButtons { NONE, START, OPTIONS, EXIT };

  // Constructor
  MainMenu();
  MainMenuButtons Update();
  void Draw();
};

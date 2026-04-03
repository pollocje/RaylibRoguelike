#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <vector>

class Map {

private:
  int cellW;
  int cellH;
  int rows;
  int cols;

  // create grid (list of vectors)
  // 1 = Wall, 0 = Floor
  std::vector<std::vector<int>> data;

public:
  // Constants for Floor vs Wall
  const int EMPTY = 0;
  const int WALL = 1;
  // Constructor
  Map(int screenWidth, int screenHeight);

  // Randomize the map (Fill with 1s and 0s)
  void initRandom();

  // First step of cellular automata
  void refine();

  // generate grid (debugging)
  void drawGrid(int width, int height);

  // Visual Map made by CA
  void drawMap();

  void generate();

  bool isCellWalkable(int targetX, int targetY);

  Vector2 getRandomFloorGridPos();

  // Returns all floor tiles reachable from (startX, startY) via flood fill
  std::vector<Vector2> getReachableFloorPositions(int startX, int startY);

  // STAIRS LOGIC
  int stairsX, stairsY;
  void spawnStairs();
  void spawnStairsInRegion(std::vector<Vector2> &reachable, int playerX, int playerY);
  void drawStairs();
  void forceFloor(int x, int y);
};

#endif

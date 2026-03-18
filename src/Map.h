#ifndef MAP_H
#define MAP_H

#include <vector>

using namespace std;

class Map {

private:
  int cellW;
  int cellH;
  int rows;
  int cols;

  // create grid (list of vectors)
  // 1 = Wall, 0 = Floor
  vector<vector<int>> data;

public:
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
};

#endif

#include "Map.h"
#include "raylib.h"
#include <cstdlib> // for random
#include <ctime>   // for time

// Constructor implementation
Map::Map(int screenWidth, int screenHeight) {
  cellW = 40;
  cellH = 40;
  cols = screenWidth / cellW;
  rows = screenHeight / cellH;

  // Init 2D Vector Class
  data.resize(rows, std::vector<int>(cols, 1));

  srand(time(NULL));

  generate();
}

void Map::generate() {
  initRandom();

  for (int i = 0; i < 5; i++) {
    refine();
  }
}

// initialize the CA and map
void Map::initRandom() {
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      // 45% chance to be a wall, otherwise floor
      data[y][x] = (rand() % 100 < 45) ? 1 : 0;
    }
  }
}

void Map::refine() {
  std::vector<std::vector<int>> nextData = data;

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      int wallCount = 0;

      // check all neighbouring cells around active cell
      for (int yy = y - 1; yy <= y + 1; yy++) {
        for (int xx = x - 1; xx <= x + 1; xx++) {
          if (yy >= 0 && yy < rows && xx >= 0 && xx < cols) {
            if (yy != y || xx != x) {
              wallCount += data[yy][xx];
            } else {
              wallCount++;
            }
          }
        }
        if (wallCount > 4)
          nextData[y][x] = 1;
        else
          nextData[y][x] = 0;
      }
    }
  }
  data = nextData;
}

void Map::drawMap() {
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      // if there is a wall
      if (data[y][x] == 1) {
        int px = x * cellW;
        int py = y * cellH;

        // Only draw a ling if the neighbour is a floor (0)
        if (y > 0 && data[y - 1][x] == 0)
          DrawLine(px, py, px + cellW, py, WHITE);
        if (y < rows - 1 && data[y + 1][x] == 0)
          DrawLine(px, py + cellH, px + cellW, py + cellH, WHITE);
        if (x > 0 && data[y][x - 1] == 0)
          DrawLine(px, py, px, py + cellH, WHITE);
        if (x < cols - 1 && data[y][x + 1] == 0)
          DrawLine(px + cellW, py, px + cellW, py + cellH, WHITE);
      }
    }
  }
}

void Map::drawGrid(int width, int height) {
  // two For loops

  // Vertical Lines
  for (int i = 0; i < width; i += cellW) {
    DrawLine(i + cellW, 0, i + cellW, height, GREEN);
  }

  // Horizontal Lines
  for (int j = 0; j < height; j += cellH) {
    DrawLine(0, j + cellH, width, j + cellH, GREEN);
  }
}

// Collision Checking for player movement
bool Map::isCellWalkable(int targetX, int targetY) {
    // Check if target is off the entire resoltuion grid
    if (targetX < 0 || targetX >= cols || targetY < 0 || targetY >= rows) {
        return false;
    }

    if (data[targetY][targetX] == 1) {
        return false;
    }

    return true;
}

#include "Map.h"
#include "GameFont.h"
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
              // Self counts as a wall if it is one, to encourage clusters
              wallCount += data[y][x];
            }
          } else {
            // Treat edges as walls
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

Vector2 Map::getRandomFloorGridPos() {
  int r, c;
  bool isGoodSpot = false;

  // Try to find a floor tile that is not isolated
  // (has at least 3 other floor neighbors)
  for (int attempt = 0; attempt < 500; attempt++) {
    c = GetRandomValue(1, cols - 2);
    r = GetRandomValue(1, rows - 2);

    if (data[r][c] == 0) {
      int floorNeighbors = 0;
      for (int yy = r - 1; yy <= r + 1; yy++) {
        for (int xx = c - 1; xx <= c + 1; xx++) {
          if (data[yy][xx] == 0)
            floorNeighbors++;
        }
      }

      if (floorNeighbors > 4) { // Needs to be in a cluster of at least 5 floor tiles (including itself)
        isGoodSpot = true;
        break;
      }
    }
  }

  // Fallback if no ideal spot found (though with CA there usually are many)
  if (!isGoodSpot) {
    do {
      c = GetRandomValue(0, cols - 1);
      r = GetRandomValue(0, rows - 1);
    } while (data[r][c] == 1);
  }

  return {(float)c, (float)r};
}

// BFS flood fill — returns every floor tile the player can actually reach
std::vector<Vector2> Map::getReachableFloorPositions(int startX, int startY) {
  std::vector<Vector2> reachable;
  std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

  // Frontier acts as a queue; we walk it with an index to avoid erasing
  std::vector<Vector2> frontier;
  frontier.push_back({(float)startX, (float)startY});
  visited[startY][startX] = true;

  int dx[4] = { 0,  0, 1, -1 };
  int dy[4] = { -1, 1, 0,  0 };

  int head = 0;
  while (head < (int)frontier.size()) {
    Vector2 current = frontier[head++];
    int cx = (int)current.x;
    int cy = (int)current.y;
    reachable.push_back(current);

    for (int d = 0; d < 4; d++) {
      int nx = cx + dx[d];
      int ny = cy + dy[d];
      if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
          !visited[ny][nx] && data[ny][nx] == 0) {
        visited[ny][nx] = true;
        frontier.push_back({(float)nx, (float)ny});
      }
    }
  }

  return reachable;
}

void Map::spawnStairs() {
  Vector2 pos = getRandomFloorGridPos();
  stairsX = (int)pos.x;
  stairsY = (int)pos.y;
}

void Map::spawnStairsInRegion(std::vector<Vector2> &reachable, int playerX, int playerY) {
  if (reachable.empty()) {
    spawnStairs(); // fallback
    return;
  }

  const int MIN_DIST = 8;
  std::vector<Vector2> farTiles;
  for (const Vector2& pos : reachable) {
    int dx = (int)pos.x - playerX;
    int dy = (int)pos.y - playerY;
    if (dx * dx + dy * dy >= MIN_DIST * MIN_DIST)
      farTiles.push_back(pos);
  }

  // Fall back to full reachable list if no tile is far enough
  std::vector<Vector2>& candidates = farTiles.empty() ? reachable : farTiles;
  int idx = rand() % (int)candidates.size();
  stairsX = (int)candidates[idx].x;
  stairsY = (int)candidates[idx].y;
}

void Map::drawStairs() {
  int px = stairsX * cellW;
  int py = stairsY * cellH;

  // Draw a purple square for stairs
  DrawRectangle(px + 5, py + 5, cellW - 10, cellH - 10, PURPLE);
  DrawTextEx(gFont, "S", {(float)(px + 12), (float)(py + 12)}, 16, 1.0f, WHITE);
}

void Map::forceFloor(int x, int y) {
  if (x >= 0 && x < cols && y >= 0 && y < rows) {
    data[y][x] = 0;
  }
}

void Map::generateWinRoom() {
  // Fill all with walls
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
      data[r][c] = WALL;

  // Carve out a large open room
  for (int r = 2; r <= 15; r++)
    for (int c = 2; c <= 22; c++)
      data[r][c] = EMPTY;
}

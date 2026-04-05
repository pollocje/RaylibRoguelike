#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"
#include <string>

enum ItemType {
    ITEM_NONE,
    ITEM_HEALTH_POTION,
    ITEM_FIREBALL_SCROLL,
    ITEM_FREEZE_SCROLL,
    ITEM_TELEPORT_SCROLL,
    ITEM_RAGE_POTION,
    ITEM_TREASURE_CHEST
};

std::string getItemName(ItemType type);
Color       getItemColor(ItemType type);
void        drawFloorItem(ItemType type, int gridX, int gridY);

// An item sitting on the dungeon floor
struct FloorItem {
    ItemType type;
    int gridX;
    int gridY;
    bool pickedUp;

    FloorItem(ItemType t, int x, int y);
};

#endif

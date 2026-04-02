#include "Item.h"

std::string getItemName(ItemType type) {
    switch (type) {
        case ITEM_HEALTH_POTION:   return "Health Potion";
        case ITEM_FIREBALL_SCROLL: return "Fireball Scroll";
        case ITEM_FREEZE_SCROLL:   return "Freeze Scroll";
        default:                   return "Unknown";
    }
}

Color getItemColor(ItemType type) {
    switch (type) {
        case ITEM_HEALTH_POTION:   return RED;
        case ITEM_FIREBALL_SCROLL: return ORANGE;
        case ITEM_FREEZE_SCROLL:   return SKYBLUE;
        default:                   return WHITE;
    }
}

void drawFloorItem(ItemType type, int gridX, int gridY) {
    int cx = gridX * 40 + 20;
    int cy = gridY * 40 + 20;
    Color col = getItemColor(type);

    switch (type) {
        case ITEM_HEALTH_POTION:
            DrawCircle(cx, cy, 7, col);
            break;

        case ITEM_FIREBALL_SCROLL: {
            // Upward-pointing triangle centered on the cell
            Vector2 top   = { (float)cx,      (float)(cy - 10) };
            Vector2 left  = { (float)(cx - 9), (float)(cy + 7) };
            Vector2 right = { (float)(cx + 9), (float)(cy + 7) };
            DrawTriangle(top, left, right, col);
            break;
        }

        case ITEM_FREEZE_SCROLL: {
            // Diamond: 4-sided polygon rotated 45 degrees
            Vector2 center = { (float)cx, (float)cy };
            DrawPoly(center, 4, 10, 45.0f, col);
            break;
        }

        default:
            DrawCircle(cx, cy, 6, WHITE);
            break;
    }
}

FloorItem::FloorItem(ItemType t, int x, int y)
    : type(t), gridX(x), gridY(y), pickedUp(false) {}

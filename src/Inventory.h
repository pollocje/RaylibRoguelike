#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"

class Inventory {
public:
    static const int GENERAL_SLOTS = 3;
    static const int MAX_POTIONS   = 3;

    ItemType slots[GENERAL_SLOTS];  // used with keys 1/2/3
    int slotCharges[GENERAL_SLOTS]; // uses remaining per slot (Mage gets 2 for scrolls)
    int healthPotions;              // dedicated HP potion stack

    Inventory();

    bool addItem(ItemType type, int charges = 1); // returns false if no room
    void removeSlot(int index);                   // decrements charges; clears at 0
    bool useHealthPotion();                  // returns false if none left
    bool hasRoom(ItemType type) const;
    bool requiresTargeting(int slot) const;  // true for fireball/freeze
};

#endif

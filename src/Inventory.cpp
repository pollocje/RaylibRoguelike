#include "Inventory.h"

Inventory::Inventory() {
    for (int i = 0; i < GENERAL_SLOTS; i++) {
        slots[i] = ITEM_NONE;
    }
    healthPotions = 0;
}

bool Inventory::hasRoom(ItemType type) const {
    if (type == ITEM_HEALTH_POTION) {
        return healthPotions < MAX_POTIONS;
    }
    for (int i = 0; i < GENERAL_SLOTS; i++) {
        if (slots[i] == ITEM_NONE) return true;
    }
    return false;
}

bool Inventory::addItem(ItemType type) {
    if (!hasRoom(type)) return false;
    if (type == ITEM_HEALTH_POTION) {
        healthPotions++;
        return true;
    }
    for (int i = 0; i < GENERAL_SLOTS; i++) {
        if (slots[i] == ITEM_NONE) {
            slots[i] = type;
            return true;
        }
    }
    return false;
}

void Inventory::removeSlot(int index) {
    if (index >= 0 && index < GENERAL_SLOTS) {
        slots[index] = ITEM_NONE;
    }
}

bool Inventory::useHealthPotion() {
    if (healthPotions <= 0) return false;
    healthPotions--;
    return true;
}

bool Inventory::requiresTargeting(int slot) const {
    if (slot < 0 || slot >= GENERAL_SLOTS) return false;
    ItemType t = slots[slot];
    return t == ITEM_FIREBALL_SCROLL || t == ITEM_FREEZE_SCROLL;
}

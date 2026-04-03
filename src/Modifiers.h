#ifndef MODIFIERS_H
#define MODIFIERS_H

enum class ModifierType {
    Speed,
    Health,
    FireRate,
    Luck,
    Dodge,
    XPGain
};

enum class ModifierTier {
    Tier1,
    Tier2,
    Tier3
};

struct Modifier {
    ModifierType type;
    ModifierTier tier;
};

// Returns the decimal bonus for a modifier (e.g. 0.10 = +10%)
float GetModifierValue(ModifierType type, ModifierTier tier);

// Display helpers for UI
const char* GetModifierTypeName(ModifierType type);
const char* GetModifierTierName(ModifierTier tier);

#endif
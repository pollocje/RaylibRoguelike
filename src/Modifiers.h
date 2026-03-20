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

// Returning the decimal bonus for the modifier
float GetModifierValue(ModifierType type, ModifierTier tier);

#endif
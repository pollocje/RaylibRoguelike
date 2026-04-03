#include "Modifiers.h"

float GetModifierValue(ModifierType type, ModifierTier tier) {
    switch (type) {
    case ModifierType::Speed:
        switch (tier) {
        case ModifierTier::Tier1: return 0.5f;
        case ModifierTier::Tier2: return 1.0f;
        case ModifierTier::Tier3: return 1.5f;
        }
        break;

    case ModifierType::Health:
        switch (tier) {
        case ModifierTier::Tier1: return 25.0f;
        case ModifierTier::Tier2: return 50.0f;
        case ModifierTier::Tier3: return 75.0f;
        }
        break;

    case ModifierType::Luck:
        switch (tier) {
        case ModifierTier::Tier1: return 0.05f;
        case ModifierTier::Tier2: return 0.10f;
        case ModifierTier::Tier3: return 0.15f;
        }
        break;

    case ModifierType::Dodge:
        switch (tier) {
        case ModifierTier::Tier1: return 0.10f;
        case ModifierTier::Tier2: return 0.15f;
        case ModifierTier::Tier3: return 0.20f;
        }
        break;

    case ModifierType::XPGain:
        switch (tier) {
        case ModifierTier::Tier1: return 0.10f;
        case ModifierTier::Tier2: return 0.20f;
        case ModifierTier::Tier3: return 0.30f;
        }
        break;
    }

    return 0.0f;
}

const char* GetModifierTypeName(ModifierType type) {
    switch (type) {
    case ModifierType::Speed:    return "Speed";
    case ModifierType::Health:   return "Health";
    case ModifierType::Luck:     return "Luck";
    case ModifierType::Dodge:    return "Dodge";
    case ModifierType::XPGain:   return "XP Gain";
    }
    return "Unknown";
}

const char* GetModifierTierName(ModifierTier tier) {
    switch (tier) {
    case ModifierTier::Tier1: return "Tier I";
    case ModifierTier::Tier2: return "Tier II";
    case ModifierTier::Tier3: return "Tier III";
    }
    return "";
}
#include "CombatLog.h"
#include "GameFont.h"
#include "raylib.h"

CombatLog::CombatLog() {}

void CombatLog::add(std::string message) {
    messages.insert(messages.begin(), message);
    if ((int)messages.size() > MAX_MESSAGES)
        messages.resize(MAX_MESSAGES);
}

void CombatLog::draw(int x, int y, int width) const {
    DrawTextEx(gFont, "Combat Log", {(float)(x + 8), (float)y}, 12, 1.0f, GRAY);
    DrawLine(x, y + 18, x + width, y + 18, DARKGRAY);

    for (int i = 0; i < (int)messages.size(); i++) {
        // Newest message is fully opaque; oldest fades to ~20% alpha
        float alpha = 1.0f - ((float)i / (float)MAX_MESSAGES) * 0.8f;
        Color c = Fade(LIGHTGRAY, alpha);
        DrawTextEx(gFont, messages[i].c_str(), {(float)(x + 8), (float)(y + 24 + i * 16)}, 9, 1.0f, c);
    }
}

void CombatLog::clear() {
    messages.clear();
}

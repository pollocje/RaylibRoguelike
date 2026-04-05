#ifndef COMBAT_LOG_H
#define COMBAT_LOG_H

#include <string>
#include <vector>

class CombatLog {
public:
    static const int MAX_MESSAGES = 12;

    CombatLog();
    void add(std::string message);
    void draw(int x, int y, int width) const;
    void clear();

private:
    std::vector<std::string> messages; // newest first
};

#endif

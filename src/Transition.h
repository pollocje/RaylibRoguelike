#ifndef TRANSITION_H
#define TRANSITION_H

#include "raylib.h"
#include <string>

class Transition {
public:
    enum class State { IDLE, FADING_OUT, TYPING, PAUSE, FADING_IN };

private:
    State state;
    float alpha;
    std::string fullText;
    std::string displayedText;
    int letterCount;
    float timer;
    float typeSpeed;
    float pauseDuration;
    bool mapSwapped;

public:
    Transition();
    void Start(std::string text);
    void Update(float dt);
    void Draw();
    
    bool IsActive() const { return state != State::IDLE; }
    bool ShouldSwapMap() { 
        if (state == State::TYPING && !mapSwapped) {
            mapSwapped = true;
            return true;
        }
        return false;
    }
    State GetState() const { return state; }
};

#endif

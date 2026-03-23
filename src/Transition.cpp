#include "Transition.h"

Transition::Transition() {
    state = State::IDLE;
    alpha = 0.0f;
    letterCount = 0;
    timer = 0.0f;
    typeSpeed = 0.1f;
    pauseDuration = 2.0f;
    mapSwapped = false;
}

void Transition::Start(std::string text) {
    fullText = text;
    displayedText = "";
    letterCount = 0;
    alpha = 0.0f;
    state = State::FADING_OUT;
    timer = 0.0f;
    mapSwapped = false;
}

void Transition::Update(float dt) {
    switch (state) {
        case State::FADING_OUT:
            alpha += dt * 1.5f; // Adjust speed as needed
            if (alpha >= 1.0f) {
                alpha = 1.0f;
                state = State::TYPING;
                timer = 0.0f;
            }
            break;

        case State::TYPING:
            timer += dt;
            if (timer >= typeSpeed) {
                timer = 0.0f;
                if (letterCount < (int)fullText.length()) {
                    letterCount++;
                    displayedText = fullText.substr(0, letterCount);
                } else {
                    state = State::PAUSE;
                    timer = 0.0f;
                }
            }
            break;

        case State::PAUSE:
            timer += dt;
            if (timer >= pauseDuration) {
                state = State::FADING_IN;
                timer = 0.0f;
            }
            break;

        case State::FADING_IN:
            alpha -= dt * 1.5f;
            if (alpha <= 0.0f) {
                alpha = 0.0f;
                state = State::IDLE;
            }
            break;

        default:
            break;
    }
}

void Transition::Draw() {
    if (state == State::IDLE) return;

    // Draw full screen black rectangle with alpha
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));

    // Draw text during TYPING and PAUSE
    if (state == State::TYPING || state == State::PAUSE || state == State::FADING_IN) {
        int fontSize = 30;
        int textWidth = MeasureText(displayedText.c_str(), fontSize);
        DrawText(displayedText.c_str(), 
                 GetScreenWidth() / 2 - textWidth / 2, 
                 GetScreenHeight() / 2 - fontSize / 2, 
                 fontSize, WHITE);
    }
}

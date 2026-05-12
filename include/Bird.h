#pragma once

#include "raylib.h"

class Bird {
public:
    Bird();

    void reset(float startX, float startY);
    void flap();
    void update(float gravity);
    void draw() const;

    Rectangle getAABB() const;
    Vector2 getPosition() const { return position_; }

private:
    Vector2 position_{};
    float velocityY_{0.0f};
    float radius_{18.0f};
};

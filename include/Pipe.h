#pragma once

#include "raylib.h"

class Pipe {
public:
    Pipe(float x, float gapCenterY, float gapSize, float width);

    void move(float dx);
    void draw(float screenHeight, Color topColor, Color bottomColor) const;

    float getX() const { return x_; }
    float getWidth() const { return width_; }
    float getGapCenterY() const { return gapCenterY_; }
    float getGapSize() const { return gapSize_; }

    Rectangle getTopAABB(float screenHeight) const;
    Rectangle getBottomAABB(float screenHeight) const;

    bool isPassed() const { return passed_; }
    void setPassed(bool v) { passed_ = v; }

private:
    float x_;
    float gapCenterY_;
    float gapSize_;
    float width_;
    bool passed_{false};
};

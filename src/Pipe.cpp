#include "Pipe.h"

Pipe::Pipe(float x, float gapCenterY, float gapSize, float width)
    : x_(x), gapCenterY_(gapCenterY), gapSize_(gapSize), width_(width) {}

void Pipe::move(float dx) { x_ += dx; }

void Pipe::draw(float screenHeight, Color topColor, Color bottomColor) const {
    const float halfGap = gapSize_ * 0.5f;
    const float topHeight = gapCenterY_ - halfGap;
    const float bottomY = gapCenterY_ + halfGap;
    const float bottomHeight = screenHeight - bottomY;

    DrawRectangle(static_cast<int>(x_), 0, static_cast<int>(width_), static_cast<int>(topHeight), topColor);
    DrawRectangle(
        static_cast<int>(x_),
        static_cast<int>(bottomY),
        static_cast<int>(width_),
        static_cast<int>(bottomHeight),
        bottomColor);
    DrawRectangleLines(static_cast<int>(x_), 0, static_cast<int>(width_), static_cast<int>(topHeight), DARKGREEN);
    DrawRectangleLines(
        static_cast<int>(x_),
        static_cast<int>(bottomY),
        static_cast<int>(width_),
        static_cast<int>(bottomHeight),
        DARKGREEN);
}

Rectangle Pipe::getTopAABB(float screenHeight) const {
    const float halfGap = gapSize_ * 0.5f;
    const float topHeight = gapCenterY_ - halfGap;
    (void)screenHeight;
    return {x_, 0.0f, width_, topHeight};
}

Rectangle Pipe::getBottomAABB(float screenHeight) const {
    const float halfGap = gapSize_ * 0.5f;
    const float bottomY = gapCenterY_ + halfGap;
    const float bottomHeight = screenHeight - bottomY;
    return {x_, bottomY, width_, bottomHeight};
}

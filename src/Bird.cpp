#include "Bird.h"

namespace {
constexpr float kFlapImpulse = -7.5f;
constexpr float kMinY = 40.0f;
}

Bird::Bird() = default;

void Bird::reset(float startX, float startY) {
    position_.x = startX;
    position_.y = startY;
    velocityY_ = 0.0f;
}

void Bird::flap() { velocityY_ = kFlapImpulse; }

void Bird::update(float gravity) {
    velocityY_ += gravity;
    position_.y += velocityY_;
    if (position_.y < kMinY + radius_) {
        position_.y = kMinY + radius_;
        velocityY_ = 0.0f;
    }
}

void Bird::draw() const {
    DrawCircleV(position_, radius_, YELLOW);
    DrawCircleLinesV(position_, radius_, GOLD);
}

Rectangle Bird::getAABB() const {
    const float d = radius_ * 2.0f;
    return {position_.x - radius_, position_.y - radius_, d, d};
}

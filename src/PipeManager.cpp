#include "PipeManager.h"
#include "DifficultyManager.h"
#include "raylib.h"

namespace {
float randomGapCenter(float screenHeight, float gapSize) {
    const float margin = 120.0f;
    const float half = gapSize * 0.5f;
    const float minY = margin + half;
    const float maxY = screenHeight - margin - half;
    if (maxY <= minY) return screenHeight * 0.5f;
    const float t = static_cast<float>(GetRandomValue(0, 10000)) / 10000.0f;
    return minY + (maxY - minY) * t;
}
}

void PipeManager::reset() {
    pipes_.clear();
    spawnAccumulator_ = 350.0f;
}

void PipeManager::trySpawn(float screenWidth, float screenHeight, const DifficultyManager& difficulty) {
    const float interval = difficulty.getPipeSpawnInterval();
    if (spawnAccumulator_ < interval) return;

    spawnAccumulator_ -= interval;
    const float gap = difficulty.getGapSize();
    const float x = screenWidth + pipeWidth_;
    pipes_.emplace_back(x, randomGapCenter(screenHeight, gap), gap, pipeWidth_);
}

void PipeManager::update(float dt, float screenWidth, float screenHeight, const DifficultyManager& difficulty) {
    const float speed = difficulty.getPipeSpeed();
    const float dx = -speed * dt * 60.0f;

    spawnAccumulator_ += speed * dt * 60.0f;
    trySpawn(screenWidth, screenHeight, difficulty);

    for (auto& p : pipes_) p.move(dx);

    const float off = -pipeWidth_ * 2.0f;
    while (!pipes_.empty() && pipes_.front().getX() + pipes_.front().getWidth() < off) {
        pipes_.erase(pipes_.begin());
    }
}

void PipeManager::draw(float screenHeight) const {
    for (const auto& p : pipes_) p.draw(screenHeight, GREEN, LIME);
}

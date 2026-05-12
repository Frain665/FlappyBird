#include "DifficultyManager.h"
#include <algorithm>

namespace {
constexpr float kGapStart = 300.0f;
constexpr float kGapEnd = 150.0f;
constexpr float kIntervalStart = 400.0f;
constexpr float kIntervalEnd = 200.0f;
constexpr float kSpeedStart = 2.0f;
constexpr float kSpeedEnd = 6.0f;
constexpr float kGravityStart = 0.3f;
constexpr float kGravityEnd = 0.8f;

constexpr int kPointsPerStep = 5;
constexpr int kStepsToMax = 10;
}

float DifficultyManager::lerp(float a, float b, float t) { return a + (b - a) * t; }

void DifficultyManager::syncToScore(int score) {
    const int steps = std::max(0, score) / kPointsPerStep;
    const float t = std::min(static_cast<float>(steps) / static_cast<float>(kStepsToMax), 1.0f);

    gapSize_ = lerp(kGapStart, kGapEnd, t);
    pipeSpawnInterval_ = lerp(kIntervalStart, kIntervalEnd, t);
    pipeSpeed_ = lerp(kSpeedStart, kSpeedEnd, t);
    gravity_ = lerp(kGravityStart, kGravityEnd, t);
}

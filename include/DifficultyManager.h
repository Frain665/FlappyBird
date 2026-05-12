#pragma once

class DifficultyManager {
public:
    void syncToScore(int score);

    float getGapSize() const { return gapSize_; }
    float getPipeSpawnInterval() const { return pipeSpawnInterval_; }
    float getPipeSpeed() const { return pipeSpeed_; }
    float getGravity() const { return gravity_; }

private:
    static float lerp(float a, float b, float t);

    float gapSize_{300.0f};
    float pipeSpawnInterval_{400.0f};
    float pipeSpeed_{2.0f};
    float gravity_{0.3f};
};

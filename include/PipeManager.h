#pragma once

#include "Pipe.h"
#include <vector>

class DifficultyManager;

class PipeManager {
public:
    void reset();
    void update(float dt, float screenWidth, float screenHeight, const DifficultyManager& difficulty);

    const std::vector<Pipe>& getPipes() const { return pipes_; }
    std::vector<Pipe>& getPipesMutable() { return pipes_; }

    void draw(float screenHeight) const;

private:
    void trySpawn(float screenWidth, float screenHeight, const DifficultyManager& difficulty);

    std::vector<Pipe> pipes_;
    float spawnAccumulator_{0.0f};
    float pipeWidth_{80.0f};
};

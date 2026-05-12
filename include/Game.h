#pragma once

#include "Bird.h"
#include "DifficultyManager.h"
#include "PipeManager.h"
#include "raylib.h"
#include <string>

class CollisionSystem {
public:
    static bool aabb(const Rectangle& a, const Rectangle& b);
};

class ScoreManager {
public:
    explicit ScoreManager(std::string recordPath);

    void resetSession();
    void addPoint();
    int getScore() const { return score_; }
    int getHighScore() const { return highScore_; }

    void onGameOverTrySaveRecord();

private:
    void loadHighScore();
    void saveHighScore() const;

    std::string recordPath_;
    int score_{0};
    int highScore_{0};
};

class Game {
public:
    Game();
    void run();

private:
    enum class Phase { Ready, Playing, Paused, GameOver };

    void resetMatch();
    void enterGameOver();
    void updatePlaying();
    void checkCollisionsAndScore();
    void drawHud() const;
    void drawReadyOverlay() const;
    void drawPauseOverlay() const;
    void drawGameOverOverlay() const;

    void loadUiFont();
    void unloadUiFont();
    void drawUiLine(const char* text, float x, float y, float fontSize, Color color) const;
    void drawUiCentered(const char* text, float centerX, float y, float fontSize, Color color) const;

    Bird bird_{};
    PipeManager pipes_{};
    DifficultyManager difficulty_{};
    ScoreManager scores_;

    Phase phase_{Phase::Ready};
    float screenW_{800.0f};
    float screenH_{600.0f};
    float birdStartX_{180.0f};
    float birdStartY_{300.0f};

    Font uiFont_{};
    bool uiFontLoaded_{false};
};

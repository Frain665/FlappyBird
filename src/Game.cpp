#include "Game.h"
#include <algorithm>
#include <fstream>
#include <vector>

namespace {

std::vector<int> buildUiCodepoints() {
    std::vector<int> out;
    out.reserve(900);
    for (int c = 32; c <= 126; ++c) out.push_back(c);
    for (int c = 0x0400; c <= 0x04FF; ++c) out.push_back(c);
    out.push_back(0x2014);
    out.push_back(0x2013);
    out.push_back(0x00AB);
    out.push_back(0x00BB);
    return out;
}

} // namespace

bool CollisionSystem::aabb(const Rectangle& a, const Rectangle& b) {
    return (a.x < b.x + b.width) && (a.x + a.width > b.x) && (a.y < b.y + b.height) && (a.y + a.height > b.y);
}

ScoreManager::ScoreManager(std::string recordPath) : recordPath_(std::move(recordPath)) { loadHighScore(); }

void ScoreManager::loadHighScore() {
    std::ifstream in(recordPath_);
    if (!in) return;
    int v = 0;
    in >> v;
    if (in.fail()) return;
    highScore_ = std::max(0, v);
}

void ScoreManager::saveHighScore() const {
    std::ofstream out(recordPath_, std::ios::trunc);
    if (!out) return;
    out << highScore_;
}

void ScoreManager::resetSession() { score_ = 0; }

void ScoreManager::addPoint() {
    ++score_;
    if (score_ > highScore_) highScore_ = score_;
}

void ScoreManager::onGameOverTrySaveRecord() {
    saveHighScore();
}

Game::Game() : scores_("flappy_record.txt") {}

void Game::loadUiFont() {
    uiFontLoaded_ = false;
    std::vector<int> cp = buildUiCodepoints();

    const char* candidates[] = {
#if defined(_WIN32)
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
#elif defined(__APPLE__)
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
#else
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
#endif
        nullptr};

    for (int i = 0; candidates[i] != nullptr; ++i) {
        if (!FileExists(candidates[i])) continue;

        Font f = LoadFontEx(candidates[i], 64, cp.data(), static_cast<int>(cp.size()));
        if (f.glyphs == nullptr || f.glyphCount <= 0 || f.texture.id == 0) {
            UnloadFont(f);
            continue;
        }
        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
        uiFont_ = f;
        uiFontLoaded_ = true;
        return;
    }

    const char* rel = TextFormat("%sassets/segoeui.ttf", GetApplicationDirectory());
    if (FileExists(rel)) {
        Font f = LoadFontEx(rel, 64, cp.data(), static_cast<int>(cp.size()));
        if (f.glyphs != nullptr && f.glyphCount > 0 && f.texture.id != 0) {
            SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
            uiFont_ = f;
            uiFontLoaded_ = true;
        } else {
            UnloadFont(f);
        }
    }
}

void Game::unloadUiFont() {
    if (!uiFontLoaded_) return;
    UnloadFont(uiFont_);
    uiFont_ = Font{};
    uiFontLoaded_ = false;
}

void Game::drawUiLine(const char* text, float x, float y, float fontSize, Color color) const {
    if (uiFontLoaded_) {
        DrawTextEx(uiFont_, text, {x, y}, fontSize, 0.0f, color);
    } else {
        DrawText(text, static_cast<int>(x), static_cast<int>(y), static_cast<int>(fontSize), color);
    }
}

void Game::drawUiCentered(const char* text, float centerX, float y, float fontSize, Color color) const {
    if (uiFontLoaded_) {
        const Vector2 sz = MeasureTextEx(uiFont_, text, fontSize, 0.0f);
        DrawTextEx(uiFont_, text, {centerX - sz.x * 0.5f, y}, fontSize, 0.0f, color);
    } else {
        const int fs = static_cast<int>(fontSize);
        const int w = MeasureText(text, fs);
        DrawText(text, static_cast<int>(centerX - static_cast<float>(w) * 0.5f), static_cast<int>(y), fs, color);
    }
}

void Game::resetMatch() {
    phase_ = Phase::Ready;
    scores_.resetSession();
    difficulty_.syncToScore(scores_.getScore());
    bird_.reset(birdStartX_, birdStartY_);
    pipes_.reset();
}

void Game::enterGameOver() {
    if (phase_ == Phase::GameOver) return;
    phase_ = Phase::GameOver;
    scores_.onGameOverTrySaveRecord();
}

void Game::updatePlaying() {
    const float dt = GetFrameTime();
    difficulty_.syncToScore(scores_.getScore());

    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) bird_.flap();

    bird_.update(difficulty_.getGravity());
    pipes_.update(dt, screenW_, screenH_, difficulty_);

    checkCollisionsAndScore();

    const Rectangle br = bird_.getAABB();
    if (br.y + br.height > screenH_ - 40.0f) enterGameOver();
}

void Game::checkCollisionsAndScore() {
    const Rectangle birdRect = bird_.getAABB();
    const Vector2 birdPos = bird_.getPosition();

    for (auto& p : pipes_.getPipesMutable()) {
        const Rectangle top = p.getTopAABB(screenH_);
        const Rectangle bottom = p.getBottomAABB(screenH_);
        if (CollisionSystem::aabb(birdRect, top) || CollisionSystem::aabb(birdRect, bottom)) {
            enterGameOver();
            return;
        }
        if (!p.isPassed() && p.getX() + p.getWidth() < birdPos.x) {
            p.setPassed(true);
            scores_.addPoint();
            difficulty_.syncToScore(scores_.getScore());
        }
    }
}

void Game::drawReadyOverlay() const {
    DrawRectangle(0, 0, static_cast<int>(screenW_), static_cast<int>(screenH_), Color{0, 0, 0, 120});

    if (uiFontLoaded_) {
        drawUiCentered(u8"Нажмите ПРОБЕЛ для старта", screenW_ * 0.5f, screenH_ * 0.5f - 55.0f, 32.0f, RAYWHITE);
        drawUiCentered(u8"В игре: ПРОБЕЛ или ЛКМ — взмах    P — пауза", screenW_ * 0.5f, screenH_ * 0.5f - 5.0f, 20.0f, LIGHTGRAY);
    } else {
        drawUiCentered("Press SPACE to start", screenW_ * 0.5f, screenH_ * 0.5f - 55.0f, 32.0f, RAYWHITE);
        drawUiCentered("SPACE or LMB - flap    P - pause", screenW_ * 0.5f, screenH_ * 0.5f - 5.0f, 20.0f, LIGHTGRAY);
    }
}

void Game::drawHud() const {
    const int s = scores_.getScore();
    const int h = scores_.getHighScore();
    drawUiLine(TextFormat("Score: %i", s), 20.0f, 20.0f, 28.0f, WHITE);
    drawUiLine(TextFormat("Record: %i", h), 20.0f, 52.0f, 22.0f, GOLD);

    drawUiLine(
        TextFormat(
            "gap: %.0f  interval: %.0f  speed: %.1f  g: %.2f",
            difficulty_.getGapSize(),
            difficulty_.getPipeSpawnInterval(),
            difficulty_.getPipeSpeed(),
            difficulty_.getGravity()),
        20.0f,
        static_cast<float>(static_cast<int>(screenH_) - 36),
        18.0f,
        LIGHTGRAY);
}

void Game::drawPauseOverlay() const {
    DrawRectangle(0, 0, static_cast<int>(screenW_), static_cast<int>(screenH_), Color{0, 0, 0, 160});
    if (uiFontLoaded_) {
        drawUiCentered(u8"ПАУЗА", screenW_ * 0.5f, screenH_ * 0.5f - 30.0f, 48.0f, RAYWHITE);
        drawUiCentered(u8"P — продолжить", screenW_ * 0.5f, screenH_ * 0.5f + 40.0f, 28.0f, RAYWHITE);
    } else {
        drawUiCentered("PAUSE", screenW_ * 0.5f, screenH_ * 0.5f - 30.0f, 48.0f, RAYWHITE);
        drawUiCentered("P - continue", screenW_ * 0.5f, screenH_ * 0.5f + 40.0f, 28.0f, RAYWHITE);
    }
}

void Game::drawGameOverOverlay() const {
    DrawRectangle(0, 0, static_cast<int>(screenW_), static_cast<int>(screenH_), Color{40, 0, 0, 180});
    if (uiFontLoaded_) {
        drawUiCentered(u8"ИГРА ОКОНЧЕНА", screenW_ * 0.5f, screenH_ * 0.5f - 80.0f, 44.0f, RAYWHITE);
        drawUiCentered(TextFormat(u8"Счёт: %i", scores_.getScore()), screenW_ * 0.5f, screenH_ * 0.5f - 10.0f, 32.0f, RAYWHITE);
        drawUiCentered(u8"Y — заново    N — выход", screenW_ * 0.5f, screenH_ * 0.5f + 50.0f, 28.0f, SKYBLUE);
    } else {
        drawUiCentered("GAME OVER", screenW_ * 0.5f, screenH_ * 0.5f - 80.0f, 44.0f, RAYWHITE);
        drawUiCentered(TextFormat("Score: %i", scores_.getScore()), screenW_ * 0.5f, screenH_ * 0.5f - 10.0f, 32.0f, RAYWHITE);
        drawUiCentered("Y - new game   N - exit", screenW_ * 0.5f, screenH_ * 0.5f + 50.0f, 28.0f, SKYBLUE);
    }
}

void Game::run() {
    InitWindow(static_cast<int>(screenW_), static_cast<int>(screenH_), "Flappy Bird");
    SetTargetFPS(60);

    loadUiFont();
    resetMatch();

    while (!WindowShouldClose()) {
        if (phase_ == Phase::Ready) {
            if (IsKeyPressed(KEY_SPACE)) {
                phase_ = Phase::Playing;
                bird_.flap();
            }
        } else if (phase_ == Phase::Playing) {
            if (IsKeyPressed(KEY_P)) {
                phase_ = Phase::Paused;
            } else {
                updatePlaying();
            }
        } else if (phase_ == Phase::Paused) {
            if (IsKeyPressed(KEY_P)) phase_ = Phase::Playing;
        } else if (phase_ == Phase::GameOver) {
            if (IsKeyPressed(KEY_Y)) {
                resetMatch();
            }
            if (IsKeyPressed(KEY_N)) break;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawRectangle(0, static_cast<int>(screenH_) - 40, static_cast<int>(screenW_), 40, DARKBROWN);

        pipes_.draw(screenH_);
        bird_.draw();
        drawHud();

        if (phase_ == Phase::Ready) drawReadyOverlay();
        if (phase_ == Phase::Paused) drawPauseOverlay();
        if (phase_ == Phase::GameOver) drawGameOverOverlay();

        EndDrawing();
    }

    unloadUiFont();
    CloseWindow();
}

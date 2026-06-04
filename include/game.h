#pragma once
#include "chicken.h"
#include "lane.h"
#include <vector>
#include <memory>
#include <chrono>
#include <string>

enum class GameState { WELCOME, PLAYING, DEAD, WIN_LEVEL, GAME_OVER };

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    // Setup
    void initNCurses();
    void initColors();
    void setupLevel();

    // Per-frame
    void processInput();
    void update(double dt);
    void render();

    // Render helpers
    void drawBackground() const;
    void drawHUD()        const;
    void drawControls()   const;
    void drawOverlay(const std::string& msg, int colorPair) const;

    void checkCollisions();

    WINDOW* _win;
    int     _wy, _wx;    // window top-left on terminal

    std::unique_ptr<Chicken>              _chicken;
    std::vector<std::unique_ptr<Lane>>    _lanes;

    int       _score;
    int       _level;
    int       _crossings; // crossings this level
    GameState _state;
    double    _stateTimer;
    bool      _quit;

    using Clock = std::chrono::steady_clock;
    std::chrono::time_point<Clock> _lastTick;
};

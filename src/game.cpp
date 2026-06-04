#include "game.h"
#include <algorithm>
#include <sstream>
#include <thread>
#include <cstring>

// ─── Window layout ───────────────────────────────────────────────────────────
// _win rows:
//   0          : top border  (box)
//   1          : HUD line
//   2          : separator
//   3 .. 13    : game rows 0..10
//   14         : bottom border (box)
// _win cols:  0 = left border, 1..GAME_W = game, GAME_W+1 = right border

static const int WIN_H   = GAME_ROWS + 4;   // 15
static const int WIN_W   = GAME_W    + 2;   // 62
static const int BASE_Y  = 3;               // game row 0 = window row 3
static const int BASE_X  = 1;               // game col 0 = window col 1

// ─── Constructor / destructor ────────────────────────────────────────────────

Game::Game()
    : _win(nullptr), _wy(0), _wx(0)
    , _score(0), _level(1), _crossings(0)
    , _state(GameState::WELCOME), _stateTimer(0)
    , _quit(false)
{
    initNCurses();
    initColors();

    int termH, termW;
    getmaxyx(stdscr, termH, termW);
    _wy = std::max(0, (termH - WIN_H) / 2);
    _wx = std::max(0, (termW - WIN_W) / 2);

    _win = newwin(WIN_H, WIN_W, _wy, _wx);
    keypad(_win, TRUE);
    wtimeout(_win, 50);   // 50 ms input timeout → ~20 fps

    _chicken = std::make_unique<Chicken>(GAME_W / 2, ROW_START);
    setupLevel();
    _lastTick = Clock::now();
}

Game::~Game() {
    if (_win) delwin(_win);
    endwin();
}

// ─── Init ────────────────────────────────────────────────────────────────────

void Game::initNCurses() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

void Game::initColors() {
    start_color();
    use_default_colors();

    // Grass rows: green text on green bg
    init_pair(CP_GRASS,  COLOR_GREEN,  COLOR_GREEN);
    // Safe median: bright yellow on yellow
    init_pair(CP_MEDIAN, COLOR_YELLOW, COLOR_YELLOW);
    // Road background: white text on black
    init_pair(CP_ROAD,   COLOR_WHITE,  COLOR_BLACK);
    // Chicken: bright yellow on transparent
    init_pair(CP_CHICK,  COLOR_YELLOW, -1);
    // Vehicles
    init_pair(CP_CAR,    COLOR_WHITE,  COLOR_RED);
    init_pair(CP_TRUCK,  COLOR_WHITE,  COLOR_BLUE);
    init_pair(CP_BUS,    COLOR_BLACK,  COLOR_CYAN);
    // UI
    init_pair(CP_HUD,    COLOR_WHITE,  COLOR_BLUE);
    init_pair(CP_BORDER, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CP_DEAD,   COLOR_RED,    -1);
    init_pair(CP_WIN,    COLOR_BLACK,  COLOR_GREEN);
    init_pair(CP_TITLE,  COLOR_BLACK,  COLOR_YELLOW);
}

void Game::setupLevel() {
    _lanes.clear();

    // Speed scales with level
    const double sp = 1.0 + (_level - 1) * 0.3;

    const std::vector<LaneCfg> cfgs = {
        { ROW_LANE1, +1, 10.0*sp, VehicleType::CAR,   CP_CAR,   2.4 },
        { ROW_LANE2, -1,  7.0*sp, VehicleType::TRUCK, CP_TRUCK, 3.2 },
        { ROW_LANE3, +1,  5.0*sp, VehicleType::BUS,   CP_BUS,   4.5 },
        { ROW_LANE4, -1, 12.0*sp, VehicleType::CAR,   CP_CAR,   2.0 },
        { ROW_LANE5, +1,  8.0*sp, VehicleType::CAR,   CP_CAR,   2.8 },
        { ROW_LANE6, -1, 11.0*sp, VehicleType::TRUCK, CP_TRUCK, 2.2 },
        { ROW_LANE7, +1,  6.0*sp, VehicleType::CAR,   CP_CAR,   3.5 },
        { ROW_LANE8, -1,  9.0*sp, VehicleType::BUS,   CP_BUS,   3.0 },
    };
    for (const auto& c : cfgs)
        _lanes.push_back(std::make_unique<Lane>(c));

    _crossings = 0;
    _chicken->revive();
}

// ─── Input ───────────────────────────────────────────────────────────────────

void Game::processInput() {
    int ch = wgetch(_win);
    if (ch == ERR) return;

    // Universal quit
    if (ch == 'q' || ch == 'Q') { _quit = true; return; }

    if (_state == GameState::WELCOME) {
        if (ch == ' ' || ch == '\n') _state = GameState::PLAYING;
        return;
    }

    if (_state == GameState::GAME_OVER) {
        if (ch == 'r' || ch == 'R') {
            _score   = 0;
            _level   = 1;
            _chicken = std::make_unique<Chicken>(GAME_W / 2, ROW_START);
            setupLevel();
            _state = GameState::PLAYING;
        }
        return;
    }

    if (_state != GameState::PLAYING || !_chicken->alive()) return;

    int dx = 0, dy = 0;
    switch (ch) {
        case KEY_UP:    case 'w': case 'W': dy = -1; break;
        case KEY_DOWN:  case 's': case 'S': dy = +1; break;
        case KEY_LEFT:  case 'a': case 'A': dx = -1; break;
        case KEY_RIGHT: case 'd': case 'D': dx = +1; break;
        default: return;
    }

    if (_chicken->move(dx, dy)) {
        if (dy < 0) _score += 10;   // reward moving forward
    }
}

// ─── Update ──────────────────────────────────────────────────────────────────

void Game::update(double dt) {
    // State transitions driven by timer
    if (_state == GameState::DEAD) {
        _stateTimer -= dt;
        if (_stateTimer <= 0) {
            if (_chicken->lives() <= 0)
                _state = GameState::GAME_OVER;
            else {
                _chicken->revive();
                _state = GameState::PLAYING;
            }
        }
        for (auto& l : _lanes) l->update(dt);
        return;
    }

    if (_state == GameState::WIN_LEVEL) {
        _stateTimer -= dt;
        if (_stateTimer <= 0) {
            _level++;
            _chicken = std::make_unique<Chicken>(GAME_W / 2, ROW_START);
            setupLevel();
            _state = GameState::PLAYING;
        }
        return;
    }

    if (_state != GameState::PLAYING) return;

    for (auto& l : _lanes) l->update(dt);
    checkCollisions();
}

void Game::checkCollisions() {
    int cx = _chicken->getX();
    int cy = _chicken->getY();

    // Win condition: reach the goal row
    if (_chicken->atGoal()) {
        _crossings++;
        _score += 100 * _level + 50 * _crossings;
        _state      = GameState::WIN_LEVEL;
        _stateTimer = 2.0;
        return;
    }

    // Only check collisions on actual road rows
    bool onRoad = (cy >= ROW_LANE1 && cy <= ROW_LANE4) ||
                  (cy >= ROW_LANE5 && cy <= ROW_LANE8);
    if (!onRoad) return;

    for (auto& l : _lanes) {
        if (l->hits(cx, cy)) {
            _chicken->die();
            _state      = GameState::DEAD;
            _stateTimer = 1.5;
            return;
        }
    }
}

// ─── Render ──────────────────────────────────────────────────────────────────

void Game::render() {
    werase(_win);
    drawBackground();

    for (auto& l : _lanes)
        l->draw(_win, BASE_Y, BASE_X);

    _chicken->draw(_win, BASE_Y, BASE_X);
    drawHUD();

    // Border
    wattron(_win, COLOR_PAIR(CP_BORDER) | A_BOLD);
    box(_win, 0, 0);
    wattroff(_win, COLOR_PAIR(CP_BORDER) | A_BOLD);

    // Separator under HUD
    wattron(_win, COLOR_PAIR(CP_BORDER));
    mvwhline(_win, 2, 1, ACS_HLINE, GAME_W);
    wattroff(_win, COLOR_PAIR(CP_BORDER));

    // Overlays
    if (_state == GameState::WELCOME)
        drawOverlay("  CHAO MUNG! Nhan SPACE de bat dau  ", CP_TITLE);
    else if (_state == GameState::WIN_LEVEL)
        drawOverlay("  *** GA DA QUA DUONG! *** Cap do moi!  ", CP_WIN);
    else if (_state == GameState::GAME_OVER)
        drawOverlay("  GAME OVER  -  Nhan R de choi lai  ", CP_DEAD);

    wrefresh(_win);
    drawControls();
    refresh();
}

void Game::drawBackground() const {
    // Grass rows
    auto fillGrass = [&](int gameRow, int cp) {
        wattron(_win, COLOR_PAIR(cp) | A_BOLD);
        for (int x = 0; x < GAME_W; ++x)
            mvwaddch(_win, BASE_Y + gameRow, BASE_X + x, ' ');
        wattroff(_win, COLOR_PAIR(cp) | A_BOLD);
    };
    fillGrass(ROW_GOAL,   CP_GRASS);
    fillGrass(ROW_MEDIAN, CP_MEDIAN);
    fillGrass(ROW_START,  CP_GRASS);

    // Labels on safe rows
    auto label = [&](int gameRow, int cp, const char* text) {
        int col = BASE_X + (GAME_W - static_cast<int>(strlen(text))) / 2;
        wattron(_win, COLOR_PAIR(cp) | A_BOLD);
        mvwaddstr(_win, BASE_Y + gameRow, col, text);
        wattroff(_win, COLOR_PAIR(cp) | A_BOLD);
    };
    label(ROW_GOAL,   CP_GRASS,  "~~~ DICH ~~~");
    label(ROW_MEDIAN, CP_MEDIAN, "--- AN TOAN ---");
    label(ROW_START,  CP_GRASS,  "~~~ XUAT PHAT ~~~");

    // Road rows: dark background + direction arrow
    for (int row = ROW_LANE1; row <= ROW_LANE8; ++row) {
        if (row == ROW_MEDIAN) continue;
        wattron(_win, COLOR_PAIR(CP_ROAD));
        for (int x = 0; x < GAME_W; ++x)
            mvwaddch(_win, BASE_Y + row, BASE_X + x, ' ');
        // Small direction hint at left margin
        // (will be overdrawn by vehicles most of the time)
        wattroff(_win, COLOR_PAIR(CP_ROAD));
    }
}

void Game::drawHUD() const {
    // Lives as chicken icons
    std::string livesStr;
    for (int i = 0; i < _chicken->lives(); ++i) livesStr += "<^> ";

    wattron(_win, COLOR_PAIR(CP_HUD) | A_BOLD);
    mvwprintw(_win, 1, 1, " SCORE: %06d  LIVES: %-12s  LEVEL: %d ",
              _score, livesStr.c_str(), _level);
    wattroff(_win, COLOR_PAIR(CP_HUD) | A_BOLD);
}

void Game::drawControls() const {
    int row = _wy + WIN_H;
    int col = _wx;
    attron(A_DIM);
    mvprintw(row, col, " [W/S/A/D] or [Arrow Keys] Move    [Q] Quit    [R] Restart ");
    attroff(A_DIM);
}

void Game::drawOverlay(const std::string& msg, int cp) const {
    int h, w;
    getmaxyx(_win, h, w);
    int row = h / 2;
    int col = std::max(1, (w - static_cast<int>(msg.size())) / 2);
    wattron(_win, COLOR_PAIR(cp) | A_BOLD | A_REVERSE);
    mvwprintw(_win, row, col, "%s", msg.c_str());
    wattroff(_win, COLOR_PAIR(cp) | A_BOLD | A_REVERSE);
}

// ─── Main loop ───────────────────────────────────────────────────────────────

void Game::run() {
    render();  // show welcome screen

    while (!_quit) {
        auto now = Clock::now();
        double dt = std::chrono::duration<double>(now - _lastTick).count();
        _lastTick = now;
        dt = std::min(dt, 0.05);  // cap to avoid physics explosions

        processInput();
        if (_quit) break;
        update(dt);
        render();
    }
}

#include "chicken.h"
#include "common.h"

Chicken::Chicken(int x, int y)
    : Entity(x, y)
    , _alive(true), _lives(3)
    , _sx(x), _sy(y), _face('^')
{}

void Chicken::draw(WINDOW* win, int baseY, int baseX) const {
    int row = baseY + _y;
    int col = baseX + _x;

    if (!_alive) {
        wattron(win, COLOR_PAIR(CP_DEAD) | A_BOLD | A_BLINK);
        mvwaddstr(win, row, col - 1, "RIP");
        wattroff(win, COLOR_PAIR(CP_DEAD) | A_BOLD | A_BLINK);
        return;
    }

    // Draw as 3-char sprite: wings + body
    char left = ' ', right = ' ';
    switch (_face) {
        case '^': left = '('; right = ')'; break;
        case 'v': left = '('; right = ')'; break;
        case '>': left = ' '; right = '>'; break;
        case '<': left = '<'; right = ' '; break;
    }
    wattron(win, COLOR_PAIR(CP_CHICK) | A_BOLD);
    mvwaddch(win, row, col - 1, left);
    mvwaddch(win, row, col,     _face);
    mvwaddch(win, row, col + 1, right);
    wattroff(win, COLOR_PAIR(CP_CHICK) | A_BOLD);
}

bool Chicken::hits(int x, int y) const {
    return _x == x && _y == y;
}

bool Chicken::atGoal() const {
    return _y == ROW_GOAL;
}

bool Chicken::move(int dx, int dy) {
    int nx = _x + dx;
    int ny = _y + dy;
    if (nx < 0 || nx >= GAME_W || ny < 0 || ny >= GAME_ROWS) return false;
    _x = nx;
    _y = ny;
    if      (dx > 0) _face = '>';
    else if (dx < 0) _face = '<';
    else if (dy < 0) _face = '^';
    else             _face = 'v';
    return true;
}

void Chicken::die() {
    _alive = false;
    if (_lives > 0) _lives--;
}

void Chicken::revive() {
    _x = _sx;
    _y = _sy;
    _alive = true;
    _face  = '^';
}

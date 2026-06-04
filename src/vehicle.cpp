#include "vehicle.h"

void Vehicle::buildSprite() {
    switch (_type) {
        case VehicleType::CAR:
            _sprite = (_dir > 0) ? "|==>|" : "|<==|"; break;
        case VehicleType::TRUCK:
            _sprite = (_dir > 0) ? "|====>|" : "|<====|"; break;
        case VehicleType::BUS:
            _sprite = (_dir > 0) ? "|=======>|" : "|<=======|"; break;
    }
}

Vehicle::Vehicle(int x, int y, int dir, double speed, VehicleType type, int color)
    : Entity(x, y)
    , _dir(dir), _speed(speed), _xf(static_cast<double>(x))
    , _color(color), _type(type)
{
    buildSprite();
}

void Vehicle::update(double dt) {
    _xf += _dir * _speed * dt;
    _x = static_cast<int>(_xf);
}

void Vehicle::draw(WINDOW* win, int baseY, int baseX) const {
    int row = baseY + _y;
    wattron(win, COLOR_PAIR(_color) | A_BOLD);
    for (int i = 0; i < static_cast<int>(_sprite.size()); ++i) {
        int col = baseX + _x + i;
        if (col >= baseX && col < baseX + GAME_W)
            mvwaddch(win, row, col, _sprite[i]);
    }
    wattroff(win, COLOR_PAIR(_color) | A_BOLD);
}

bool Vehicle::hits(int x, int y) const {
    if (y != _y) return false;
    return x >= _x && x < _x + static_cast<int>(_sprite.size());
}

bool Vehicle::offscreen() const {
    if (_dir > 0) return _x >= GAME_W;
    return _x + static_cast<int>(_sprite.size()) <= 0;
}

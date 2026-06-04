#include "lane.h"

Lane::Lane(const LaneCfg& cfg)
    : _cfg(cfg)
    , _timer(0)
    , _rng(std::random_device{}())
    , _dist(cfg.interval * 0.5, cfg.interval * 1.8)
{
    // Pre-populate: 2 vehicles staggered across the lane
    int gap = GAME_W / 3;
    for (int i = 0; i < 2; ++i) {
        int sx = (_cfg.dir > 0) ? (i * gap + 2) : (GAME_W - 2 - i * gap);
        _vehicles.push_back(std::make_unique<Vehicle>(
            sx, _cfg.row, _cfg.dir, _cfg.speed, _cfg.vtype, _cfg.color));
    }
    _timer = _dist(_rng);
}

void Lane::spawn() {
    // Spawn just off the edge the vehicle enters from
    int sx = (_cfg.dir > 0) ? -10 : GAME_W + 1;
    _vehicles.push_back(std::make_unique<Vehicle>(
        sx, _cfg.row, _cfg.dir, _cfg.speed, _cfg.vtype, _cfg.color));
}

void Lane::update(double dt) {
    for (auto& v : _vehicles) v->update(dt);

    // Remove vehicles that have left the screen
    _vehicles.erase(
        std::remove_if(_vehicles.begin(), _vehicles.end(),
            [](const auto& v) { return v->offscreen(); }),
        _vehicles.end());

    _timer -= dt;
    if (_timer <= 0) {
        spawn();
        _timer = _dist(_rng);
    }
}

void Lane::draw(WINDOW* win, int baseY, int baseX) const {
    for (const auto& v : _vehicles)
        v->draw(win, baseY, baseX);
}

bool Lane::hits(int x, int y) const {
    for (const auto& v : _vehicles)
        if (v->hits(x, y)) return true;
    return false;
}

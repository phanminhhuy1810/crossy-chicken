#pragma once
#include "vehicle.h"
#include <vector>
#include <memory>
#include <random>

struct LaneCfg {
    int         row;
    int         dir;
    double      speed;
    VehicleType vtype;
    int         color;
    double      interval;  // avg seconds between spawns
};

class Lane {
public:
    explicit Lane(const LaneCfg& cfg);

    void update(double dt);
    void draw(WINDOW* win, int baseY, int baseX) const;
    bool hits(int x, int y) const;

private:
    LaneCfg                              _cfg;
    std::vector<std::unique_ptr<Vehicle>> _vehicles;
    double                               _timer;
    std::mt19937                         _rng;
    std::uniform_real_distribution<double> _dist;

    void spawn();
};

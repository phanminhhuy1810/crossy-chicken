#pragma once
#include "entity.h"
#include <string>

class Vehicle : public Entity {
public:
    Vehicle(int x, int y, int dir, double speed, VehicleType type, int colorPair);

    void update(double dt)                              override;
    void draw(WINDOW* win, int baseY, int baseX) const  override;
    bool hits(int x, int y) const                       override;

    bool offscreen() const;
    int  length()    const { return static_cast<int>(_sprite.size()); }

private:
    int         _dir;     // +1 right, -1 left
    double      _speed;   // columns / second
    double      _xf;      // precise float position
    int         _color;
    VehicleType _type;
    std::string _sprite;

    void buildSprite();
};

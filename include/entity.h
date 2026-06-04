#pragma once
#include "common.h"

// Abstract base for every game object
class Entity {
public:
    Entity(int x, int y) : _x(x), _y(y) {}
    virtual ~Entity() = default;

    virtual void update(double dt)                              = 0;
    virtual void draw(WINDOW* win, int baseY, int baseX) const = 0;
    virtual bool hits(int x, int y) const                      = 0;

    int getX() const { return _x; }
    int getY() const { return _y; }

protected:
    int _x, _y;
};

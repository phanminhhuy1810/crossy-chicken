#pragma once
#include "entity.h"

class Chicken : public Entity {
public:
    Chicken(int x, int y);

    void update(double /*dt*/)                         override {}
    void draw(WINDOW* win, int baseY, int baseX) const override;
    bool hits(int x, int y) const                      override;

    bool move(int dx, int dy);
    void die();
    void revive();

    bool alive()  const { return _alive; }
    int  lives()  const { return _lives; }
    bool atGoal() const;

private:
    bool _alive;
    int  _lives;
    int  _sx, _sy;   // spawn position
    char _face;      // '^' 'v' '<' '>'
};

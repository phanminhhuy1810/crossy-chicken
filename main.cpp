#include "game.h"
#include <exception>
#include <cstdio>

int main() {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        endwin();
        fprintf(stderr, "Loi: %s\n", e.what());
        return 1;
    }
    return 0;
}

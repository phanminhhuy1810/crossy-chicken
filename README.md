# Crossy Chicken

A C++ terminal game built with ncurses. Guide a chicken across eight traffic lanes to the goal, using the safe median between the two halves of the road. Cars, trucks, and buses move in alternating directions, with faster traffic at each new level.

The current version includes scoring, three lives per level, collision detection, and a restart option after game over. The game uses text sprites and Vietnamese interface labels.

This was an early programming exercise developed with AI assistance.

## Build and run

Requirements:

- A C++17 compiler and `make`.
- ncurses headers and library.
- A terminal with color support, at least 62 columns wide and 16 rows tall.

From the repository root:

```sh
make
./ga_qua_duong
```

You can also use `make run` to build and launch the game. Use `make clean` to remove the executable.

## Controls

| Key | Action |
| --- | --- |
| Space or Enter | Start from the welcome screen |
| Arrow keys or W / A / S / D | Move up / left / down / right |
| R | Restart after game over |
| Q | Quit from any screen |

Letter keys work in either case. Moving upward earns points; reaching the goal awards a bonus and starts the next level. A collision costs one life and returns the chicken to the starting row if lives remain.

## Source layout

- `main.cpp` starts the game.
- `src/game.cpp` handles the game loop, input, states, and rendering.
- `src/chicken.cpp`, `src/vehicle.cpp`, and `src/lane.cpp` implement the player and traffic.
- `include/` contains the class declarations and shared settings.

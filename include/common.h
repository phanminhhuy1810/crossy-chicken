#pragma once
#include <ncurses.h>

// ─── Game dimensions ────────────────────────────────────────────────────────
static const int GAME_W    = 60;   // playfield width in columns
static const int GAME_ROWS = 11;   // row indices 0..10

// ─── Row layout ─────────────────────────────────────────────────────────────
static const int ROW_GOAL   = 0;   // finish (green grass)
static const int ROW_LANE1  = 1;   // →  fast cars
static const int ROW_LANE2  = 2;   // ←  medium trucks
static const int ROW_LANE3  = 3;   // →  slow buses
static const int ROW_LANE4  = 4;   // ←  fast cars
static const int ROW_MEDIAN = 5;   // safe zone (yellow grass)
static const int ROW_LANE5  = 6;   // →  medium cars
static const int ROW_LANE6  = 7;   // ←  fast trucks
static const int ROW_LANE7  = 8;   // →  slow cars
static const int ROW_LANE8  = 9;   // ←  medium buses
static const int ROW_START  = 10;  // start (green grass)

// ─── Color-pair IDs ─────────────────────────────────────────────────────────
static const int CP_GRASS  = 1;
static const int CP_ROAD   = 2;
static const int CP_CHICK  = 3;
static const int CP_CAR    = 4;
static const int CP_TRUCK  = 5;
static const int CP_BUS    = 6;
static const int CP_HUD    = 7;
static const int CP_BORDER = 8;
static const int CP_DEAD   = 9;
static const int CP_WIN    = 10;
static const int CP_MEDIAN = 11;
static const int CP_TITLE  = 12;

enum class VehicleType { CAR, TRUCK, BUS };

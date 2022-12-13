#pragma once

#include "stuff.h"

typedef struct Coordinate {
    int x;
    int y;
} Coordinate;

Coordinate Coordinate_add(Coordinate a, Coordinate b) {
    Coordinate c = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return c;
}

Coordinate Coordinate_minus(Coordinate a, Coordinate b) {
    Coordinate c = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return c;
}

bool Coordinate_equal(Coordinate a, Coordinate b) {
    return a.x == b.x && a.y == b.y;
}

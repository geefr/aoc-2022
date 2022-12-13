#pragma once

#include "stuff.h"
#include "coord.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef struct HeightMap {
    int* data;
    size_t width;
    size_t height;
} HeightMap;

HeightMap HeightMap_create(int w, int h) {
    HeightMap m = {
        .width = w,
        .height = h,
        .data = (int*)calloc(w * h, sizeof(int))
    };
    return m;
}
void HeightMap_destroy(HeightMap m) {
    free(m.data);
}

int HeightMap_CoordinateIsOnEdge(HeightMap m, Coordinate c) {
    return(
        (size_t)c.x == 0 || (size_t)c.x == m.width - 1 ||
        (size_t)c.y == 0 || (size_t)c.y == m.height - 1
    );
}
int HeightMap_CoordinateIsValid(HeightMap m, Coordinate c) {
    if(c.x < 0 || (size_t)c.x > m.width - 1) return false;
    if(c.y < 0 || (size_t)c.y > m.height - 1) return false;
    return true;
}

int HeightMap_get(HeightMap m, Coordinate p) {
    if( !HeightMap_CoordinateIsValid(m, p) ) return -INT_MAX;
    return m.data[(p.y * m.width) + p.x];
}
void HeightMap_set(HeightMap m, Coordinate p, int v) {
    if( !HeightMap_CoordinateIsValid(m, p) ) return;
    m.data[(p.y * m.width) + p.x] = v;
}

Coordinate HeightMap_find(HeightMap m, int v) {
    for( size_t y = 0; y < m.height; ++y ) {
        for( size_t x = 0; x < m.width; ++x ) {
            Coordinate p = {.x = x, .y = y};
            if(HeightMap_get(m, p) == v) return p;
        }
    }

    Coordinate invalid = {.x = -1, .y = -1};
    return invalid;
}


int HeightMap_IsVisibleFromDirection(HeightMap m, Coordinate c, Coordinate direction, int* viewDistance) {
    *viewDistance = 0;

    // if at the e
    if( HeightMap_CoordinateIsOnEdge(m, c) ) return true;
    int v = HeightMap_get(m, c);

    Coordinate c2 = Coordinate_add(c, direction);
    while( HeightMap_CoordinateIsValid(m, c2) ) {
        int v2 = HeightMap_get(m, c2);
        ++(*viewDistance);
        if( v2 >= v ) return false;
        c2 = Coordinate_add(c2, direction);
    }
    return true;
}

void printHeightMap(HeightMap m) {
    for(size_t y = 0; y < m.height; ++y) {
        for(size_t x = 0; x < m.width; ++x) {
            Coordinate c = {.x = x, .y = y};
            printf("%d", HeightMap_get(m, c));
        }
        printf("\n");
    }
}

void printHeightMap_char(HeightMap m) {
    for(size_t y = 0; y < m.height; ++y) {
        for(size_t x = 0; x < m.width; ++x) {
            Coordinate c = {.x = x, .y = y};
            printf("%c", HeightMap_get(m, c));
        }
        printf("\n");
    }
}
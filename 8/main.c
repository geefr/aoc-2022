
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128
#define MAXPATH 128
#define MAXCHILDREN 128

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define TRUE 1
#define FALSE 0

typedef struct HeightMap {
    int* data;
    size_t width;
    size_t height;
} HeightMap;

typedef struct Visibility {
    int visUp;
    int visRight;
    int visDown;
    int visLeft;
} Visibility;

typedef struct Coordinate {
    int x;
    int y;
} Coordinate;

HeightMap HeightMap_create(int w, int h) {
    HeightMap m = {
        .width = w,
        .height = h,
        .data = calloc(w * h, sizeof(int))
    };
    return m;
}
void HeightMap_destroy(HeightMap m) {
    free(m.data);
}
int HeightMap_get(HeightMap m, Coordinate p) {
    return m.data[(p.y * m.width) + p.x];
}
void HeightMap_set(HeightMap m, Coordinate p, int v) {
    m.data[(p.y * m.width) + p.x] = v;
}
Coordinate Coordinate_add(Coordinate a, Coordinate b) {
    Coordinate c = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return c;
}

void parseLine(char* line) {
    if(line == NULL) {
        printf("Invalid Input\n");
        return;
    }
    size_t length = strlen(line);
    if( line[length - 1] == '\n' ) {
        line[length - 1] = '\0';
        length = strlen(line);
    }   
}
int HeightMap_CoordinateIsOnEdge(HeightMap m, Coordinate c) {
    return(
        (size_t)c.x == 0 || (size_t)c.x == m.width - 1 ||
        (size_t)c.y == 0 || (size_t)c.y == m.height - 1
    );
}
int HeightMap_CoordinateIsValid(HeightMap m, Coordinate c) {
    if(c.x < 0 || (size_t)c.x > m.width - 1) return FALSE;
    if(c.y < 0 || (size_t)c.y > m.height - 1) return FALSE;
    return TRUE;
}
int HeightMap_IsVisibleFromDirection(HeightMap m, Coordinate c, Coordinate direction, int* viewDistance) {
    *viewDistance = 0;

    // if at the e
    if( HeightMap_CoordinateIsOnEdge(m, c) ) return TRUE;
    int v = HeightMap_get(m, c);

    Coordinate c2 = Coordinate_add(c, direction);
    while( HeightMap_CoordinateIsValid(m, c2) ) {
        int v2 = HeightMap_get(m, c2);
        ++(*viewDistance);
        if( v2 >= v ) return FALSE;
        c2 = Coordinate_add(c2, direction);
    }
    return TRUE;
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

void problems(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    size_t w = 0;
    size_t h = 0;
    while (fgets(buffer, MAXLEN, f))
    {
        w = MAX(w, strlen(buffer) - 1);
        h++;
    }
    printf("Creating %lux%lu map\n", w, h);
    HeightMap m = HeightMap_create(w, h);
    fseek(f, 0, SEEK_SET);

    size_t y = 0;
    while (fgets(buffer, MAXLEN, f))
    {
        for( size_t x = 0; x < strlen(buffer); ++x ) {
            if( buffer[x] == '\n' ) continue;
            char v = buffer[x] - '0';
            Coordinate c = {.x = x, .y = y};
            HeightMap_set(m, c, v);
        }
        y++;
    }

    int maxScenic = 0;
    int visible = 0;
    for(size_t y = 0; y < m.height; ++y)
    {
        int viewUp, viewRight, viewDown, viewLeft;
        for(size_t x = 0; x < m.width; ++x) {
            Coordinate c = {.x = x, .y = y};
            Coordinate up = {.x = 0, .y = -1};
            Coordinate right = {.x = 1, .y = 0};
            Coordinate down = {.x = 0, .y = 1};
            Coordinate left = {.x = -1, .y = 0};
            int visUp = HeightMap_IsVisibleFromDirection(m, c, up, &viewUp);
            int visRight = HeightMap_IsVisibleFromDirection(m, c, right, &viewRight);
            int visDown = HeightMap_IsVisibleFromDirection(m, c, down, &viewDown);
            int visLeft = HeightMap_IsVisibleFromDirection(m, c, left, &viewLeft);

            if( visUp || visRight || visDown || visLeft )
            {
                // if( !HeightMap_CoordinateIsOnEdge(m, c) ) {
                // printf("%lu,%lu = visible\n", x, y);
                // }
                visible++;

                
                int scenic = viewUp * viewRight * viewDown * viewLeft;
                // printf("view distances: %lu,%lu : %d,%d,%d,%d = %d\n", x, y, viewUp, viewRight, viewDown, viewLeft, scenic);
                maxScenic = MAX(maxScenic, scenic);
            }
        }
    }
    printf("Problem 1: %d\n", visible);
    printf("Problem 2: %d\n", maxScenic);

    HeightMap_destroy(m);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("USAGE: %s input.txt\n", argv[0]);
        return 1;
    }

    const char *inputFilePath = argv[1];

    FILE *inputFile = fopen(inputFilePath, "r");
    if (!inputFile)
    {
        printf("Error: Failed to open file: %s", inputFilePath);
        return 1;
    }

    problems(inputFile);
    fclose(inputFile);

    return 0;
}

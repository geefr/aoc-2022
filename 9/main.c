
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128
#define MAXPATH 128
#define MAXCHILDREN 128

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
typedef int bool;
#define true 1
#define false 0

#include "../common/coord.h"

#define Vec_Type Coordinate
#include "../common/vec.h"

void moveHead(Coordinate* headPos, Coordinate move) {
    *headPos = Coordinate_add(*headPos, move);
}

void moveTail(Coordinate* tailPos, Coordinate headPos) {
    Coordinate d = Coordinate_minus(headPos, *tailPos);
    if( abs(d.x) <= 1 && abs(d.y) <= 1 ) {
        // head covers tail, or right next to - Nothing to do
        return;
    }
    // UDLR
    if( d.y == 0 && abs(d.x) == 2 ) {
        if( d.x > 0 ) {
            tailPos->x++;
        } else {
            tailPos->x--;
        }
    }
    else if( d.x == 0 && abs(d.y) == 2) {
        if( d.y > 0 ) {
            tailPos->y++;
        } else {
            tailPos->y--;
        }
    }
    else {
        // Tail moves diagonally in some way
        Coordinate diagonalMoves[4];
        diagonalMoves[0].x = -1;
        diagonalMoves[0].y = 1;
        diagonalMoves[1].x = 1;
        diagonalMoves[1].y = 1;
        diagonalMoves[2].x = -1;
        diagonalMoves[2].y = -1;
        diagonalMoves[3].x = 1;
        diagonalMoves[3].y = -1;
        
        for( int i = 0; i < 4; ++i ) {
            Coordinate move = diagonalMoves[i];
            Coordinate newD = Coordinate_minus(headPos, Coordinate_add(*tailPos, move));
            if( abs(newD.x) <= 1 && abs(newD.y) <= 1 ) {
                *tailPos = Coordinate_add(*tailPos, move);
                break;
            }
        }
    }
}

void problem1(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    Vec visitedPositions = Vec_create();

    Coordinate headPos = {.x = 0, .y = 0};
    Coordinate tailPos = {.x = 0, .y = 0};
    Vec_push_back(&visitedPositions, tailPos);
    while (fgets(buffer, MAXLEN, f))
    {
        char direction;
        int distance;
        size_t length = strlen(buffer);
        if( buffer[length - 1] == '\n' ) {
            buffer[length - 1] = '\0';
            length = strlen(buffer);
        }   

        direction = buffer[0];
        distance = atoi(buffer + 2);

        Coordinate move = {.x = 0, .y = 0};
        switch(direction)
        {
            case 'U':
                move.y = 1;
                break;
            case 'R':
                move.x = 1;
                break;
            case 'D':
                move.y = -1;
                break;
            case 'L':
                move.x = -1;
                break;
        }

        printf("Head %d,%d\nTail %d,%d\nMove %d,%d\nDistance %d\n", 
            headPos.x, headPos.y,
            tailPos.x, tailPos.y,
            move.x, move.y, distance
        );
        for( int i = 0; i < distance; ++i ) {
            moveHead(&headPos, move);
            moveTail(&tailPos, headPos);
            bool alreadyVisited = false;
            for( size_t i = 0; i < visitedPositions.size; ++i ) {
                if( visitedPositions.items[i].x == tailPos.x &&
                    visitedPositions.items[i].y == tailPos.y ) {
                        alreadyVisited = true;
                        break;
                    }
            }
            if( !alreadyVisited ) {
                Vec_push_back(&visitedPositions, tailPos);
            }
        }
        printf("Head %d,%d\nTail %d,%d\n\n", 
            headPos.x, headPos.y,
            tailPos.x, tailPos.y
        );
    }
    
    printf("Problem 1: %lu\n", visitedPositions.size);

    Vec_destroy(&visitedPositions);
}

void problem2(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    Vec visitedPositions = Vec_create();

    int numKnots = 10;
    Coordinate knots[numKnots];
    for(int k = 0; k < numKnots; ++k ) {
        knots[k].x = 0;
        knots[k].y = 0;
    }

    Vec_push_back(&visitedPositions, knots[numKnots - 1]);
    while (fgets(buffer, MAXLEN, f))
    {
        char direction;
        int distance;
        size_t length = strlen(buffer);
        if( buffer[length - 1] == '\n' ) {
            buffer[length - 1] = '\0';
            length = strlen(buffer);
        }   

        direction = buffer[0];
        distance = atoi(buffer + 2);

        Coordinate move = {.x = 0, .y = 0};
        switch(direction)
        {
            case 'U':
                move.y = 1;
                break;
            case 'R':
                move.x = 1;
                break;
            case 'D':
                move.y = -1;
                break;
            case 'L':
                move.x = -1;
                break;
        }

        // printf("Head %d,%d\nTail %d,%d\nMove %d,%d\nDistance %d\n", 
        //     headPos.x, headPos.y,
        //     tailPos.x, tailPos.y,
        //     move.x, move.y, distance
        // );
        for( int i = 0; i < distance; ++i ) {
            moveHead(knots + 0, move);
            for( int knot = 1; knot < numKnots; ++knot ) {
                moveTail(knots + knot, knots[knot - 1]);
            }

            bool alreadyVisited = false;
            Coordinate tailPos = knots[numKnots - 1];
            for( size_t i = 0; i < visitedPositions.size; ++i ) {
                if( visitedPositions.items[i].x == tailPos.x &&
                    visitedPositions.items[i].y == tailPos.y ) {
                        alreadyVisited = true;
                        break;
                    }
            }
            if( !alreadyVisited ) {
                Vec_push_back(&visitedPositions, tailPos);
            }
        }
        // printf("Head %d,%d\nTail %d,%d\n\n", 
        //     headPos.x, headPos.y,
        //     tailPos.x, tailPos.y
        // );
    }
    
    printf("Problem 2: %lu\n", visitedPositions.size);

    Vec_destroy(&visitedPositions);
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

    // problem1(inputFile);
    // fseek(inputFile, 0, SEEK_SET);
    problem2(inputFile);
    fclose(inputFile);

    return 0;
}

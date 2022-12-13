
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAXLEN 256

#include "../common/stuff.h"
#include "../common/coord.h"
#include "../common/heightmap.h"

#define Vec_Type Coordinate
#include "../common/vec.h"

#define Tree_Type Coordinate
#define Tree_maxchildren 4
#include "../common/tree.h"

Coordinate left = {.x = -1, .y = 0};
Coordinate up = {.x = 0, .y = -1};
Coordinate right = {.x = 1, .y = 0};
Coordinate down = {.x = 0, .y = 1};

void validDirections(HeightMap m, Coordinate from, bool *leftValid, bool *upValid, bool *rightValid, bool *downValid)
{
    int fromHeight = HeightMap_get(m, from);

    if (from.x < 0)
        *leftValid = false;
    else
        *leftValid = abs(HeightMap_get(m, Coordinate_add(from, left)) - fromHeight) <= 1;
    if (from.y < 0)
        *upValid = false;
    else
        *upValid = abs(HeightMap_get(m, Coordinate_add(from, up)) - fromHeight) <= 1;
    if ((size_t)from.x >= m.width)
        *rightValid = false;
    else
        *rightValid = abs(HeightMap_get(m, Coordinate_add(from, right)) - fromHeight) <= 1;
    if ((size_t)from.y >= m.height)
        *downValid = false;
    else
        *downValid = abs(HeightMap_get(m, Coordinate_add(from, down)) - fromHeight) <= 1;
}

Tree* treeFindUpwards(Tree* node, Coordinate c)
{
    if( node == NULL ) return node;
    if( Coordinate_equal(node->value, c) ) return node;
    return treeFindUpwards(node->parent, c);
}

void destroyTreeNodesUntilNotRootOrNotLeaf(Tree* node) {
    while( node->parent && node->numChildren == 0 ) {
        Tree* toDelete = node;
        node = node->parent;
        Tree_removeChild(node, toDelete);
        Tree_destroy(toDelete);
    }
}

bool walkRoutes(HeightMap m, Coordinate from, Coordinate to, Coordinate previous, Tree* routeTree)
{
    static int i = 0;
    if( i++ % 1000 == 0 ) {
        printf("walRoute(%d)\n", i);
    }

    // printf("Walking tree from %d,%d\n", from.x, from.y);
    bool lv, uv, rv, dv;
    validDirections(m, from, &lv, &uv, &rv, &dv);

    bool nodeValid = true;

    if( routeTree->parent )
    {
        if( treeFindUpwards(routeTree->parent, from) )
        {
            // Already visited this location as part of this route
            nodeValid = false;
        }
    }

    if (!lv && !uv && !rv && !dv)
    {
        nodeValid = false;
    }
    
    if( !nodeValid && routeTree->parent ) {
        // A dead end or similar, drop from the tree
        // TODO: Doesn't work - Just drop this node, then see hack in problems()
        // destroyTreeNodesUntilNotRootOrNotLeaf(routeTree);
        Tree_removeChild(routeTree->parent, routeTree);
        Tree_destroy(routeTree);
        return false;
    }

    if( Coordinate_equal(from, to) )
    {
        // Hooray!
        return true;
    }



    nodeValid = false;
    // Problem 1 - Go right unless we can't
    if (rv)
    {
        Coordinate next = Coordinate_add(from, right);
        if (!Coordinate_equal(next, previous))
        {
            Tree* nextStep = Tree_createNode(next);
            Tree_addChild(routeTree, nextStep);
            nodeValid |= walkRoutes(m, next, to, from, nextStep);
            if( nodeValid ) return true;
        }
    }

    if (lv)
    {
        Coordinate next = Coordinate_add(from, left);
        if (!Coordinate_equal(next, previous))
        {
            Tree* nextStep = Tree_createNode(next);
            Tree_addChild(routeTree, nextStep);
            nodeValid |= walkRoutes(m, next, to, from, nextStep);
        }
    }

    if (uv)
    {
        Coordinate next = Coordinate_add(from, up);
        if (!Coordinate_equal(next, previous))
        {
            Tree* nextStep = Tree_createNode(next);
            Tree_addChild(routeTree, nextStep);
            nodeValid |= walkRoutes(m, next, to, from, nextStep);
        }
    }

    if (dv)
    {
        Coordinate next = Coordinate_add(from, down);
        if (!Coordinate_equal(next, previous))
        {
            Tree* nextStep = Tree_createNode(next);
            Tree_addChild(routeTree, nextStep);
            nodeValid |= walkRoutes(m, next, to, from, nextStep);
        }
    }

    if( !nodeValid ) {
        Tree_removeChild(routeTree->parent, routeTree);
        Tree_destroy(routeTree);
        return false;
    }

    return true;
}

bool deleteTreeRoutesWhichDontReach( Tree* node, Coordinate end ) {
    // TODO: This function doesn't work quite right - Need to repeatedly call it to actually prune things..
    bool anyDeleted = false;
    for( size_t i = 0; i < node->numChildren; ++i ) {
        anyDeleted |= deleteTreeRoutesWhichDontReach(node->children[i], end);
    }

    if( node->parent && node->numChildren == 0 ) {
        if( !Coordinate_equal(node->value, end) ) {
            // This is the leaf of a route which doesn't reach the end
            Tree_removeChild(node->parent, node);
            Tree_destroy(node);
            anyDeleted = true;
        }
    }

    return anyDeleted;
}

int findShortestRouteInTree( Tree* node, Coordinate end, int distance ) {
    // printf("Find shortest %d (%d,%d) [%lu]\n", distance, node->value.x, node->value.y, (long unsigned int)node);

    if( node->numChildren == 0 )
    {
        if( !Coordinate_equal(node->value, end) )
        {
            printf("Error: Non-end leaf node found in tree!!\n");
            exit(123);
        }
        return distance;
    }

    int shortest = INT_MAX;
    distance++;
    for( size_t i = 0; i < node->numChildren; ++i ) {
        shortest = MIN(shortest, findShortestRouteInTree(node->children[i], end, distance));
    }

    return shortest;
}

void printRouteTree(Tree* node, int indent)
{
    for( int i = 0; i < indent; ++i ) printf(" ");
    printf("(%d, %d)\n", node->value.x, node->value.y);
    
    for( size_t i = 0; i < node->numChildren; ++i ) {
        printRouteTree(node->children[i], indent + 1);
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
    // printf("Creating %lux%lu map\n", w, h);
    HeightMap m = HeightMap_create(w, h);
    fseek(f, 0, SEEK_SET);

    size_t y = 0;
    while (fgets(buffer, MAXLEN, f))
    {
        for (size_t x = 0; x < strlen(buffer); ++x)
        {
            if (buffer[x] == '\n')
                continue;
            char v = buffer[x];
            Coordinate c = {.x = x, .y = y};
            HeightMap_set(m, c, v);
        }
        y++;
    }

    // Find all the routes we could take, in some recursive way    
    Coordinate startCoord = HeightMap_find(m, 'S');
    Coordinate endCoord = HeightMap_find(m, 'E');
    if (!HeightMap_CoordinateIsValid(m, startCoord) ||
        !HeightMap_CoordinateIsValid(m, endCoord))
    {
        printf("Couldn't find start / end points\n");
        exit(1);
    }

    Tree* routeTree = Tree_createNode(startCoord);

    HeightMap_set(m, startCoord, 'a');
    HeightMap_set(m, endCoord, 'z');

    walkRoutes(m, startCoord, endCoord, startCoord, routeTree);

    // printf("BEFORE PRUNE\n\n");
    // printRouteTree(routeTree, 0);

    // This function doesn't work quite right yet..
    // for( int hack = 0; hack < 1000; ++hack ) {
    //     deleteTreeRoutesWhichDontReach(routeTree, endCoord);
    // }

    // printf("AFTER PRUNE\n\n");
    // printRouteTree(routeTree, 0);

    printf("Shortest route: %d\n", findShortestRouteInTree(routeTree, endCoord, 0));

    HeightMap_destroy(m);
    Tree_destroy(routeTree);
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


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef Tree_Type
# error You must #define Tree_Type <your type> to use Tree
#endif
#ifndef Tree_maxchildren
# error You must #define Tree_maxchildren n to use Tree
#endif

// TODO: Thou shalt only have one type of tree in thy module
// #define _tree Tree##Tree_Type

typedef struct Tree Tree;
struct Tree
{
    Tree_Type value;

    Tree* children[Tree_maxchildren];
    size_t numChildren;
    Tree* parent;
};

Tree* Tree_createNode(Tree_Type v) {
    Tree* n = malloc(sizeof(Tree));
    n->value = v;

    for(size_t i = 0; i < Tree_maxchildren; ++i) {
        n->children[0] = NULL;
    }
    n->numChildren = 0;
    n->parent = NULL;
    return n;
}
void Tree_addChild(Tree* parent, Tree* child) {
    parent->children[parent->numChildren] = child;
    parent->numChildren++;
    child->parent = parent;
}
void Tree_removeChild(Tree* parent, Tree* child) {
    size_t childIndex = -1;
    bool childFound = false;
    for( size_t i = 0; i < parent->numChildren; ++i ) {
        if( parent->children[i] == child ) {
            childIndex = i;
            childFound = true;
        }
    }
    if( childFound )
    {
        parent->children[childIndex] = NULL;
        for( size_t i = childIndex; i < parent->numChildren - 1; ++i ) {
            parent->children[i] = parent->children[i+1];
        }
        parent->numChildren--;
    }
}

void Tree_destroy(Tree* node) {
    if( node == NULL ) return;
    for(size_t i = 0; i < node->numChildren; ++i) {
        Tree_destroy(node->children[i]);
    }
    free(node);
}

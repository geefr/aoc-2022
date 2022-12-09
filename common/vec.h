#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef Vec_Type
# error You must #define Vec_Type <your type> to use Vec
#endif

// TODO: Thou shalt only have one type of vec in thy module
// #define _vec Vec##Vec_Type

typedef struct Vec {
  Vec_Type* items;
  size_t size;
} Vec;
Vec Vec_create() { Vec s = {.items = NULL, .size = 0}; return s; }
void Vec_destroy(Vec* s) {free(s->items);}
void Vec_push_back(Vec* s, Vec_Type v) {
    Vec_Type* oldItems = s->items;
    size_t oldSize = s->size;
    s->size++;
    s->items = malloc(s->size * sizeof(Vec_Type));
    if( oldItems ) {
        memcpy(s->items, oldItems, oldSize * sizeof(Vec_Type));
        free(oldItems);
    }
    s->items[s->size - 1] = v;
}
void Vec_pop_back(Vec* s) {
    if( s->size == 0 ) return;
    Vec_Type v = s->items[s->size - 1];
    Vec_Type* oldItems = s->items;
    s->size--;
    if( s->size > 0 ) {
        s->items = malloc(s->size * sizeof(Vec_Type));
        memcpy(s->items, oldItems, s->size * sizeof(Vec_Type));
    } else {
        s->items = NULL;
    }
    free(oldItems);
}
void Vec_reverse(Vec* s) {
    if( s->size == 0 ) return;
    Vec_Type* oldItems = s->items;
    s->items = malloc(s->size * sizeof(Vec_Type));
    for( int i = s->size - 1, j = 0; i >= 0; --i, ++j ) {
        s->items[j] = oldItems[i];
    }
    free(oldItems);
}

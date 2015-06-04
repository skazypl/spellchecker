#ifndef SET_H
#define SET_H 

#include <wchar.h>
#include <stdbool.h>

#define MAX_ALPH_SIZE 40

struct InsertSet
{
    short int size;
    wchar_t array[MAX_ALPH_SIZE];
};

void set_init(struct InsertSet* s);

int set_add(struct InsertSet* s, wchar_t wc);

void set_done(struct InsertSet* s);

#endif
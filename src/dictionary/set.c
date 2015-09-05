/** @file
    Implementacja zbioru elementów typu wchar_h
    używanych potem do proponowania podpowiedzi.

    @ingroup dictionary
    @author Jarosław Socha <js347267@students.mimuw.edu.pl>
    */

#include "set.h"
#include <stdlib.h>

/** @name Elementy interfejsu 
   @{
 */

void set_init(struct InsertSet* s)
{
    s->size = 0;
    s->buffer_size = 1;
    s->array = malloc(sizeof(wchar_t));
}

void set_add(struct InsertSet* s, wchar_t wc)
{
    for (int i = 0; i < s->size; ++i)
        if(s->array[i] == wc)
            return;

    ++(s->size);
    if(s->size > s->buffer_size)
    {   
        wchar_t *newArray = 
            malloc(sizeof(wchar_t) * s->buffer_size * 2);

        for (int i = 0; i < s->size - 1; ++i)
            newArray[i] = s->array[i];
        for (int i = s->buffer_size; i < 2 * s->buffer_size; ++i)
            newArray[i] = L'\0';
        
        s->buffer_size *= 2;
        free(s->array);
        s->array = newArray;
    }
    s->array[s->size - 1] = wc;
}

void set_done(struct InsertSet* s)
{
    free(s->array);
}

///@}
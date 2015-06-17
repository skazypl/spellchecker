#include "set.h"

/** @name Elementy interfejsu 
   @{
 */

void set_init(struct InsertSet* s)
{
    s->size = 0;
}

int set_add(struct InsertSet* s, wchar_t wc)
{
    if(s->size == MAX_ALPH_SIZE)
        return 0;
    bool already = false;
    for (short int i = 0; i < s->size; ++i)
        if (s->array[i] == wc)
        {
            already = true;
            break;
        }
        
    if(!already)
    {
        s->array[s->size] = wc;
        s->size++;
    }
    return 1;
}

void set_done(struct InsertSet* s)
{
}

///@}
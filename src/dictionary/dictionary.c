/** @file
  Prosta implementacja słownika.
  Słownik przechowuje listę słów.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-05-11
  @todo Napisać efektywną implementację.
 */

#include "trie.h"
#include "dictionary.h"
#include "word_list.h"
#include <stdio.h>
#include <wctype.h>
#include <stdlib.h>
#include <assert.h>

#define _GNU_SOURCE

#define MAX_ALPH_SIZE 50

/**
  Struktura przechowująca słownik.
  Uzywamy drzewa trie
 */
struct dictionary
{
    struct Tree* tree; ///< Drzewo przechowujące słowa w słowniku.
    struct InsertSet* usedLetters;
};

/** @name Funkcje pomocnicze
  @{
 */
/**
  Czyszczenie pamięci słownika
  @param[in,out] dict słownik
 */
static void dictionary_free(struct dictionary *dict)
{
    Tree_destroy(dict->tree);
}

wchar_t* decapitalize(const wchar_t* word)
{
    wchar_t* smallWord =
        (wchar_t*)malloc((wcslen(word) + 2) * sizeof(wchar_t));
    wcscpy(smallWord, word);
    for (int i = 0; i < wcslen(smallWord); ++i)
    {
        smallWord[i] = towlower(word[i]);
    }
    return smallWord;
}

struct InsertSet
{
    int size;
    wchar_t array[MAX_ALPH_SIZE];
};

void set_init(struct InsertSet* s)
{
    s->size = 0;
}

int set_add(struct InsertSet* s, wchar_t wc)
{
    if(s->size == MAX_ALPH_SIZE)
        return 0;
    bool already = false;
    for (int i = 0; i < s->size; ++i)
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

/**@}*/
/** @name Elementy interfejsu 
  @{
 */
struct dictionary * dictionary_new()
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
    dict->tree = (struct Tree*)malloc(sizeof(struct Tree));
    dict->usedLetters = (struct InsertSet*)malloc(sizeof(struct InsertSet));
    Tree_init(dict->tree);
    set_init(dict->usedLetters);  
    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    dictionary_free(dict);
    free(dict->usedLetters);
    free(dict->tree);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{  
    wchar_t* smallWord = decapitalize(word);
    if (dictionary_find(dict, smallWord))
    {
        free(smallWord);
        return 0;
    }
    for (int i = 0; i < wcslen(smallWord); ++i) //+1? zAraz ise przekonamy
        if (set_add(dict->usedLetters, smallWord[i]) == 0)
            return 0;

    add(dict->tree, smallWord);
    free(smallWord);
    return 1;
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    wchar_t* smallWord = decapitalize(word);
    delete(dict->tree, smallWord);
    free(smallWord);
    return 1;
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    //wchar_t* lowerWord = (wchar_t*)malloc(sizeof(word) + sizeof(wchar_t));
    //for (int i = 0; i < wcslen(word); ++i)
        //lowerWord[i] = towlower(word[i]);
    wchar_t* smallWord = decapitalize(word);
    if (find(dict->tree, smallWord) == 0)
    {
        free(smallWord);
        return false;
    }
    free(smallWord);
    return true;
}

int dictionary_save(const struct dictionary *dict, FILE* stream)
{
  return Tree_save(dict->tree, stream);
}

struct dictionary * dictionary_load(FILE* stream)
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
    dict->tree = Tree_load(stream);
    if(dict->tree == NULL)
        return NULL;
    return dict;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
        struct word_list *list)
{

  
    word_list_init(list);
    wchar_t* smallWord = decapitalize(word);
    size_t wlen = wcslen(smallWord);

    assert('A' < 'Z');
    assert('Z' < 'a');
    assert('a' < 'z');

    for (size_t i = 0; i < wlen; ++i)
    {
        wchar_t *begin, *end;

        //zastepujemy i-ty znak w slowie word na wszystkie inne mniejsze znaki
        
        for (int j = 0; j < dict->usedLetters->size; j++)
        {
            wchar_t start = dict->usedLetters->array[j];
            if (start == smallWord[i])
                continue;

            wchar_t* newWordChange =
                (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
            wcscpy(newWordChange, word);
            newWordChange[i] = start;
            if(dictionary_find(dict, newWordChange))
                word_list_add(list, newWordChange);
            free(newWordChange);
        }
        

        //dodajemy --przed-- kazda litera w word kazdy mozliwy znak
        //(w tym przed nieistniejaca na pozycji word[wlen])

        begin = (wchar_t*)malloc((i + 1 + 1) * sizeof(wchar_t));
        //jeden wchar na \0 a drugi na dodawany nowy wchar na koncu
        wcsncpy(begin, smallWord, i);
        begin[i] = '\0';

        end = (wchar_t*)malloc((wlen - i + 1) * sizeof(wchar_t));
        wcsncpy(end, smallWord + i, wlen - i);
        end[wlen - i] = '\0';

        wchar_t* newWordAdd = (wchar_t*)malloc((wlen + 1 + 1) * sizeof(wchar_t));

        wcscpy(newWordAdd, begin);
        newWordAdd[i] = 'a'; //dowolna litera, wazna inicjalizacja
        newWordAdd[i + 1] = '\0';
        wcscat(newWordAdd, end);
        
        for (int j = 0; j < dict->usedLetters->size; j++)
        {
            wchar_t start = dict->usedLetters->array[j];
            newWordAdd[i] = start;            

            if(dictionary_find(dict, newWordAdd))
                word_list_add(list, newWordAdd);
        }
        free(newWordAdd);

        //i recznie za nieistniejacym znakiem
        if (i == wlen - 1)
        {   
            for (int j = 0; j < dict->usedLetters->size; j++)
            {
                wchar_t start = dict->usedLetters->array[j];
                newWordAdd = (wchar_t*)malloc((wlen + 1 + 1) * sizeof(wchar_t));
                wcscpy(newWordAdd, smallWord);
                newWordAdd[wlen] = start;
                newWordAdd[wlen + 1] = '\0';
                if(dictionary_find(dict, newWordAdd))
                    word_list_add(list, newWordAdd);
                free(newWordAdd);
            }
        }

        //usuwamy ity znak - czyli zmieniamy tylko end

        free(end);
        end = (wchar_t*)malloc((wlen - i) * sizeof(wchar_t));
        wcsncpy(end, smallWord + i + 1, wlen - i - 1);
        end[wlen - i - 1] = '\0';

        wchar_t* newWordDel = (wchar_t*)malloc((wlen) * sizeof(wchar_t));
        wcscpy(newWordDel, begin);
        wcscat(newWordDel, end);
        if(dictionary_find(dict, newWordDel))
            word_list_add(list, newWordDel);

        free(newWordDel);
        free(begin);
        free(end);

    }

    free(smallWord);
}

/**@}*/

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
#include <stdlib.h>
#include <assert.h>

#define _GNU_SOURCE

/**
  Struktura przechowująca słownik.
  Uzywamy drzewa trie
 */
struct dictionary
{
    struct Tree* tree; ///< Drzewo przechowujące słowa w słowniku.
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

/**@}*/
/** @name Elementy interfejsu 
  @{
 */
struct dictionary * dictionary_new()
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
    dict->tree = (struct Tree*)malloc(sizeof(struct Tree));
    Tree_init(dict->tree);
    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    dictionary_free(dict);
    free(dict->tree);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    if (dictionary_find(dict, word))
        return 0;
    add(dict->tree, word);
    return 1;
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    /// @bug `struct word_list` nie obsługuje operacji usuwania.
    return 0;
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
  if (find(dict->tree, word) == 0)
    return false;
  return true;
}

int dictionary_save(const struct dictionary *dict, FILE* stream)
{
  return Tree_save(dict->tree, stream);
}

struct dictionary * dictionary_load(FILE* stream)
{
    struct dictionary *dict = dictionary_new();
    dict->tree = Tree_load(stream);
    return dict;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
        struct word_list *list)
{

  
    word_list_init(list);
    size_t wlen = wcslen(word);
    //wordUncapital = dekapitalizuj(word);

    assert('A' < 'Z');
    assert('Z' < 'a');
    assert('a' < 'z');

    for (size_t i = 0; i < wlen; ++i)
    {
        wchar_t *begin, *end;

        //zastepujemy i-ty znak w slowie word na wszystkie inne mniejsze znaki
        
        for (wchar_t start = 'a'; start <= 'z'; start++)
        {
            if (start == word[i])
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
        wcsncpy(begin, word, i);
        begin[i] = '\0';

        end = (wchar_t*)malloc((wlen - i + 1) * sizeof(wchar_t));
        wcsncpy(end, word + i, wlen - i);
        end[wlen - i] = '\0';

        wchar_t* newWordAdd = (wchar_t*)malloc((wlen + 1 + 1) * sizeof(wchar_t));

        wcscpy(newWordAdd, begin);
        newWordAdd[i] = 'a'; //dowolna litera, wazna inicjalizacja
        newWordAdd[i + 1] = '\0';
        wcscat(newWordAdd, end);
        
        for (wchar_t start = 'a'; start <= 'z'; start++)
        {
            newWordAdd[i] = start;   
            if(dictionary_find(dict, newWordAdd))
                word_list_add(list, newWordAdd);
        }
        free(newWordAdd);

        //i recznie za nieistniejacym znakiem
        if (i == wlen - 1)
        {   
            for (wchar_t start = 'a'; start <= 'z'; start++)
            {
                newWordAdd = (wchar_t*)malloc((wlen + 1 + 1) * sizeof(wchar_t));
                wcscpy(newWordAdd, word);
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
        wcsncpy(end, word + i + 1, wlen - i - 1);
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

}

/**@}*/

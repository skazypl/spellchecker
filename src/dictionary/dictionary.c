/** @file
  Prosta implementacja słownika.
  Słownik przechowuje listę słów.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-05-11
 */

#include "trie.h"
#include "dictionary.h"
#include "word_list.h"
#include <stdio.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define _GNU_SOURCE


/**
  Struktura przechowująca słownik.
  Uzywamy drzewa trie
 */
struct dictionary
{
    struct Tree* tree; ///< Drzewo przechowujące słowa w słowniku.
    struct InsertSet* usedLetters; ///< Zbiór wszystkich liter ze słownika.
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

/**
    Zamiana wszystkich wielkich liter w słowie na małe.
    @param[in] word Słowo z małymi i być może wielkimi literami.
    @return Słowo złożone tylko z małych liter.
    */

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

/**
    Zamienia miejscami w tablicy wchar-ów dwa elementy o zadanych indeksach. 
    @param[in] a, b Indeksy zamienianych elementów.
    @param[in, out] array Tablica.
    */

void swap(wchar_t** array, int a, int b)
{
    wchar_t* aWord = array[a];
    wchar_t* bWord = array[b];
    array[a] = bWord;
    array[b] = aWord;
}


/**
    Komparator dla qsorta w zastępstwie alphaInsertSorta
    @param[in] a, b Wskaźniki na wide stringi.
    @return Wynik funkcji wcscoll - dobra funkcja dla komparatora.
    */
int wcharComp (const void * a, const void * b)
{
    const wchar_t* a_ = *(const wchar_t**)a;
    const wchar_t* b_ = *(const wchar_t**)b;
    return wcscoll(a_, b_);
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
    set_done(dict->usedLetters);
    free(dict->usedLetters);
    free(dict->tree);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{  
    if(wcslen(word) == 0)
    {
        return 0;
    }
    wchar_t* smallWord = decapitalize(word);
    if (dictionary_find(dict, smallWord))
    {
        free(smallWord);
        return 0;
    }
    for (int i = 0; i < wcslen(smallWord); ++i)
        if (set_add(dict->usedLetters, smallWord[i]) == 0) 
        //jesli nie ma takiej litery
            return 0;
        

    add(dict->tree, smallWord);
    free(smallWord);
    return 1;
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    wchar_t* smallWord = decapitalize(word);
    int toReturn = delete(dict->tree, smallWord);
    free(smallWord);
    return toReturn;
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    wchar_t* smallWord = decapitalize(word);
    for (int i = 0; i < wcslen(smallWord); ++i)
        if(!iswalpha(smallWord[i]))
            return false;

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
  return Tree_save_DFS(dict->tree, stream);
}

struct dictionary * dictionary_load(FILE* stream)
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
    dict->tree = Tree_load_DFS(stream);
    if(dict->tree == NULL)
    {
        free(dict);
        return NULL;
    }
    dict->usedLetters = usedInTree(dict->tree);
    return dict;
}


void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
        struct word_list *list)
{
    struct word_list newListObj;
    struct word_list* newList = &newListObj;
    word_list_init(newList);

    wchar_t* smallWord = decapitalize(word);
    size_t wlen = wcslen(smallWord);

    assert('A' < 'Z');
    assert('Z' < 'a');
    assert('a' < 'z');

    if(dictionary_find(dict, word))
        word_list_add(newList, word);

    for (size_t i = 0; i < wlen; ++i)
    {
        wchar_t *begin, *end;

        //zastepujemy ity znak w slowie word na wszystkie inne mniejsze znaki
        
        for (int j = 0; j < dict->usedLetters->size; j++)
        {
            wchar_t start = dict->usedLetters->array[j];
            if (start == smallWord[i])
                continue;

            wchar_t* newWordChange =
                (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
            wcscpy(newWordChange, smallWord);
            newWordChange[i] = start;
            if(dictionary_find(dict, newWordChange))
                word_list_add(newList, newWordChange);
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

        wchar_t* newWordAdd =
            (wchar_t*)malloc((wlen + 1 + 1) * sizeof(wchar_t));

        wcscpy(newWordAdd, begin);
        newWordAdd[i] = 1; // dowolna litera, wazna inicjalizacja. 
        ///<  @todo Usunąć lub obejść.
        newWordAdd[i + 1] = '\0';
        wcscat(newWordAdd, end);
        
        for (int j = 0; j < dict->usedLetters->size; j++)
        {
            wchar_t start = dict->usedLetters->array[j];
            newWordAdd[i] = start;            

            if(dictionary_find(dict, newWordAdd))
                word_list_add(newList, newWordAdd);
        }
        free(newWordAdd);

        //i recznie za nieistniejacym znakiem
        if (i == wlen - 1)
        {   
            for (int j = 0; j < dict->usedLetters->size; j++)
            {
                wchar_t start = dict->usedLetters->array[j];
                newWordAdd =
                    (wchar_t*)malloc((wlen + 1 + 1) * sizeof(wchar_t));
                wcscpy(newWordAdd, smallWord);
                newWordAdd[wlen] = start;
                newWordAdd[wlen + 1] = '\0';
                if(dictionary_find(dict, newWordAdd))
                    word_list_add(newList, newWordAdd);
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
            word_list_add(newList, newWordDel);

        free(newWordDel);
        free(begin);
        free(end);

    }

    free(smallWord);

    wchar_t* newArray[newList->size];
    for (int i = 0; i < newList->size; ++i)
        newArray[i] = newList->array[i];

    qsort(newArray, newList->size, sizeof(wchar_t*), wcharComp);

    word_list_init(list);  
    for (int i = 0; i < newList->size; ++i)
    {
        if(i == 0)
            word_list_add(list, newArray[i]);
        else
            if(wcscmp(newArray[i], newArray[i-1]) != 0)
                word_list_add(list, newArray[i]);
    }
    word_list_done(newList);
}


void addLang(char** buffer, const char* lang, int* lastZero)
{
    int langLeng = strlen(lang);
    for (int i = 0; i < langLeng; ++i)
        (*buffer)[i + *lastZero + 1] = lang[i];

    *lastZero += (langLeng + 1);

    

}


int dictionary_lang_list(char **list, size_t *list_len)
{
    struct stat st = {0};
    const char* dirPath = "dict_database/";

    if(stat(dirPath, &st) == -1)
    {
        return -1;
    }
    else
    {
        DIR* d = opendir(dirPath);
        struct dirent* dir;
        *list_len = 0;
        if(d)
        {
            *list = NULL;//calloc(sizeof(char) , 50); //usunac
            while((dir = readdir(d)) != NULL)
            {
                if(strcmp(dir->d_name, ".") != 0 && 
                    (strcmp(dir->d_name, "..") != 0))
                    *list_len += (strlen(dir->d_name) + 1);
            }
            *list = (char*)calloc(sizeof(char), *list_len);
            rewinddir(d);
            int lastZero = -1;
            while((dir = readdir(d)) != NULL)
            {
                if(strcmp(dir->d_name, ".") != 0 && 
                    (strcmp(dir->d_name, "..") != 0))
                {
                    addLang(list, dir->d_name, &lastZero);
                }
            }

            closedir(d);
        }
    }
    for (int i = 0; i < *list_len; ++i)
    {
    }
    return 0;
}


struct dictionary * dictionary_load_lang(const char *lang)
{
    const char* dirPath = "dict_database/"; // nie wiem czy nie ./
    char* langPath = 
        calloc(sizeof(char), (strlen(dirPath) + strlen(lang) + 1));
    strcat(langPath, dirPath);
    strcat(langPath, lang);

    struct stat st = {0};
    if(stat(langPath, &st) != -1)
    {
        FILE* stream = fopen(langPath, "r");
        if(stream == NULL)
        {
            free(langPath);
            return NULL;
        }
        struct dictionary* toReturn;
        if((toReturn = dictionary_load(stream)) == NULL)
        {
            fclose(stream);
            free(langPath);
            return NULL;
        }
        fclose(stream);
        free(langPath);
        return toReturn;
    }

    free(langPath);
    return NULL;
}


int dictionary_save_lang(const struct dictionary *dict, const char *lang)
{
    struct stat st = {0};
    const char* dirPath = "dict_database/";
    char* langPath = 
        calloc(sizeof(char), (strlen(dirPath) + strlen(lang) + 1));
    strcat(langPath, dirPath);
    strcat(langPath, lang);

    mkdir(dirPath, S_IRWXU);
    if(stat(dirPath, &st) == -1)
    {
        free(langPath);
        return -1;
    }    
    
    FILE* stream = fopen(langPath, "w");
    if(stream == NULL)
    {
        free(langPath);
        return -1;
    }
    if(dictionary_save(dict, stream) != 0)
    {
        fclose(stream);
        free(langPath);
        return -1;
    }
    fclose(stream);
    free(langPath);
    return 0;
}
    

/**@}*/

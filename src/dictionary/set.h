/** @file
	Interfejs zbioru z możliwością dodawania elementów.

    @ingroup dictionary
    @author Jarosław Socha <js347267@students.mimuw.edu.pl>
 */

#ifndef SET_H
#define SET_H 

#include <wchar.h>
#include <stdbool.h>

#define MAX_ALPH_SIZE 50 ///< Założenie o maksymalnym rozmiarze alfabetu

/**
	Struktura zbioru z możliwością wstawiania elementów typu wchar.
	*/

struct InsertSet
{
    short int size; ///< Liczba elementów zbioru.
    wchar_t array[MAX_ALPH_SIZE]; ///< Tablica przechowująca elementy zbioru.
};


/**
	Inicjalizacja zbioru.
	Zbiór niszczy się metodą set_done.
	@param[in, out] s Zbiór.
	*/

void set_init(struct InsertSet* s);

/**
	Dodaje element do zbioru.
	@param[in, out] s Zbiór.
	@param[in] wc Litera dodawana do zbioru.	
	*/

int set_add(struct InsertSet* s, wchar_t wc);

/**
	Niszczy	zbiór zainicjalizowany w set_init.
	@param[in] s Zbiór.
	*/

void set_done(struct InsertSet* s);

#endif
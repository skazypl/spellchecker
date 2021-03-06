/** @file
	Interfejs drzewa trie do przechowywania słownika.

	@ingroup dictionary
	@author Jarosław Socha <js347267@students.mimuw.edu.pl>
	*/

#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include "set.h"

/**
	Struktura pojedynczego węzła w drzewie.
	*/

struct Node
{
	wchar_t key; ///< Klucz identyfikujący węzeł. Dla liści znak pusty tzn \0.
	struct Node* parent; ///< Wskaźnik do ojca danego węzła. NULL dla roota.
	struct Node** children; /**< Tablica wskaźników do synów typu Node.
	Synowie nie są ułożeni alfabetycznie.*/
	short int childCount; ///< Liczba synów węzła.
};

/**
	Struktura pełnego drzewa, zawierająca tylko wskaźnik do typu węzłowego.
	*/

struct Tree
{
	struct Node* root;	/**< Wskaźnik do korzenia drzewa typu węzłowego Node.
	Węzeł atrapa - nie przechowywujemy w nim klucza.
	*/
};

/**
	Inicjalizacja drzewa.
	Drzewo niszczy się za pomocą Tree_destroy().
	@param[in, out] t Drzewo.
	*/

void Tree_init(struct Tree *t);

/**
	Destrukcja drzewa.
	Zwalnia całą zaalokowaną pamięć.
	@param[in] t Drzewo.
	*/

void Tree_destroy(struct Tree *t);

/**
	Dodaje słowo do drzewa.	
	@param[in, out] t Drzewo.
	@param[in] word Słowo wstawiane do drzewa.
	@return 1 jeśli udało się wstawić, 0 wpp.
	*/

int Tree_add(struct Tree *t, const wchar_t* word);

/**
	Sprawdza czy podane słowo jest już w drzewie.
	@param[in, out] t Drzewo.
	@param[in] word Poszukiwane słowo.
	@return 1 jeśli słowo jest w drzewie, 0 jeśli nie ma.
	*/

int Tree_find(struct Tree *t, const wchar_t* word);

/**
	Usuwa słowo ze słownika.
	@param[in, out] t Drzewo.
	@param[in] word Usuwane słowo.
	@return 1 jeśli udało się usunąć słowo, 0 jeśli nie.
	*/
int Tree_delete(struct Tree *t, const wchar_t* word);

/**
	Zwraca zbiór typu InsertSet (z nagłówka set) 
	zawierający litery użyte w drzewie, bez powtórzeń.
	@param[in] t Drzewo.
	@return Zbiór użytych liter w drzewie.
	*/
struct InsertSet* usedInTree(struct Tree* t);

/**
	Ładuje ze strumienia plikowy zapis drzewa i zwraca wskaźnik do korzenia.
	@param[in] stream Plik z zapisanym drzewem.
	@return Drzewo wczytane z pliku.
	*/
struct Tree* Tree_load_DFS(FILE* stream);

/**
	Zapisuje drzewo do pliku.

	konwencja przechowywania drzewa na dysku: ciąg par
	<wchar_t - klucz node'a><int - liczba potomków>
	dla liscia: "!0".

	@param[in] t Drzewo.
	@param[in] stream Plik docelowy.
	@return 0 jeśli udało się zapisać, <0 wpp.
	*/
int Tree_save_DFS(struct Tree* t, FILE* stream);

/**
	Zwraca liczbę węzłów w drzewie.

	Należy pamiętać, że drzewo zawiera puste węzły jako liście oraz
	że root jest pustym węzłem.

	@param[in] t Drzewo.
	@return Liczba węzłów w drzewie.
	*/

int Tree_size(struct Tree* t);


#endif /* TRIE_H */
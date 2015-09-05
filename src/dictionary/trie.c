/** @file
	Implementacja drzewa trie do przechowywania słownika.

	@ingroup dictionary
	@author Jarosław Socha <js347267@students.mimuw.edu.pl>
	*/

#include "trie.h"
#include "io_mock.h"
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/** @name Funkcje pomocnicze.
   @{
 */

/**
	Inicjalizacja węzła po jego utworzeniu.
	@param[in, out] n Węzeł drzewa.
   */

static void Node_init(struct Node *n)
{
	n->key = L'\0';
	n->parent = NULL;
	n->children = NULL;
	n->childCount = 0;
}

/**
	Tworzy węzeł o przekazanych przez argument kluczu, liczbie dzieci i rodzicu.
	@param[in] childCount Liczba dzieci i rozmiar tablicy dzieci do zaalokownia.
	@param[in] key Klucz typu wchar_t nowego węzła.
	@param[in] parent Wskaźnik do węzła rodzica.
	@return Wskaźnik do stworzonego węzła, NULL gdy nie udało się stworzyć.

	*/

static struct Node* createNode(int childCount, wchar_t key, struct Node* parent)
{
	if(childCount < 0)
		return NULL;

	struct Node* toReturn = (struct Node*)malloc(sizeof(struct Node));
	Node_init(toReturn);

	toReturn->childCount = childCount;
	toReturn->parent = parent;
	toReturn->key = key;

	if(childCount != 0)
		toReturn->children = (struct Node**)
			malloc(sizeof(struct Node*) * childCount);
	else
		toReturn->children = NULL;

	for (int i = 0; i < childCount; ++i)
		toReturn->children[i] = NULL;

	return toReturn;
}

/**
	Niszczy (rekurencyjnie) węzeł i wszystkie węzły potomne.
	Zwalnia całą zaalokowaną pamięć, w tym tę przeznaczoną na węzeł.
	@param[in] n Wskaźnik do węzła.
	*/


static void Node_destroy(struct Node *n)
{
	if(n != NULL)
	{	
		for (int i = 0; i < n->childCount; ++i)
			Node_destroy(n->children[i]);
		
		free(n->children);
		free(n);
	}
}

/**
	Komparator na dwóch węzłach drzewa.
	Komparator ocenia relację < na podstawie ich wartości key.
	@param[in] a, b Wskaźniki do węzłów.
	@return 0 jeśli węzły mają identyczne klucze, 1 jeśli większy jest klucz
	pierwszego argumentu, -1 jeśli drugiego.

	*/

static int nodeComp(const void* a, const void* b)
{
	struct Node* a_ = *(struct Node**)a;
	struct Node* b_ = *(struct Node**)b;
	if(a_->key < b_->key)
		return -1;
	else 
		if(a_->key == b_->key)
			return 0;
		else
			return 1;
}

/**
	Wyszukiwanie binarne węzła o zadanym kluczu w zadanej argumentem tablicy.
	Tablica jest z założenia posortowana rosnąco 
	(w sensie komparatora @ref nodeComp).
	@param [in] arr[] Tablica
	@param [in] size Rozmiar przeszukiwanej tablicy.
	@param [in] patt Klucz, którego pociadacza poszukujemy.
	@return indeks tablicy, ktorego element jest rowny poszukiwanemu, -1 jeśli
	nie ma takiego.
	*/

static int binSearch(struct Node* arr[], int size, wchar_t patt)
{
	if(size < 1)
		return -1;

	int beg = 0;
	int end = size - 1;
	while(beg < end)
	{
		int mid = (beg + end) / 2;
		if(arr[mid]->key == patt)
			return mid;

		if(arr[mid]->key < patt)
			beg = mid + 1;
		else
			end = mid;
	}
	if(arr[beg]->key == patt)
		return beg;
	return -1;
} 

/**
	Dodaje węzeł jako potomka węzła przekazanego przez argument.
	Następnie wywołuje się rekurencyjnie obcinając pierwszy znak ze słowa 
	word, dopóki nie jest ono puste.

	@param[in] n Wskaźnik na węzeł.
	@param[in] word Pozostałe słowo wstawiane znak po znaku.
	@return 1 w przypadku sukcesu.
	@todo przerobić na void lub dodać inne zwracane wartości.
	*/

static int addNode(struct Node *n, const wchar_t* word)
{
	if(wcslen(word) != 0)
	{
		int ind = binSearch(n->children, n->childCount, word[0]);
		if(ind != -1)
			return addNode(n->children[ind], ++word);
	}

	//jezeli nie znaleziono lub wstawiamy puste slowo (wiec lisc)
	n->childCount++;
	struct Node* newNode = (struct Node*)(malloc(sizeof(struct Node)));
	Node_init(newNode);

	struct Node** newChildren =
		(struct Node**)(malloc(n->childCount * sizeof(struct Node*)));
	for (int i = 0; i < n->childCount - 1; ++i)
		newChildren[i] = n->children[i];

	newChildren[n->childCount - 1] = newNode;	
	free(n->children);
	n->children = newChildren;

	newNode->parent = n;
	newNode->key = word[0];

	qsort(n->children, n->childCount, sizeof(struct Node*), nodeComp);

	if(wcslen(word) != 0)
	{
		word++;
		return addNode(newNode, word);
	}
	else
		return 1;
}

/**
	Sprawdza czy istnieje w drzewie ścieżka w dół argumentowanego węzła,
	która układa się w słowo z argumentu - zatem czy słowo jest w drzewie
	ukorzenionym w zadanym węźle.

	Nie należy mylić z wyszukiwaniem prefiksu - słowo z argumentu kończy się
	znakiem \0, więc funkcja zwróci 1 wtw gdy słowo jest w słowniku.

	@param[in] n Wskaźnik na węzeł.
	@param[in] word Słowo.
	@return 0 jeśli słowa nie ma w drzewie ukorzenionym w węźle, 1 jeśli jest.
	*/


static int findNode(struct Node* n, const wchar_t* word)
{
	int ind = binSearch(n->children, n->childCount, word[0]);
	if(ind != -1)
	{
		if(word[0] == L'\0')
			return 1;
		else
		{
			word++;
			return findNode(n->children[ind], word);
		}
	}

	return 0;
}

/**
	Zwraca wskaźnik do liścia zadanego słowa, jeśli to znajduje się w słowniku.
	@param[in] n Wskaźnik na węzeł.
	@param[in] word słowo.
	@return Wskaźnik na liść jeśli słowo jest w słowniku, NULL wpp.
	*/

static struct Node* findLeaf(struct Node* n, const wchar_t* word)
{
	int ind = binSearch(n->children, n->childCount, word[0]);
	if(ind != -1)
	{
		if(word[0] == L'\0')
			return n->children[ind];
		return findLeaf(n->children[ind], ++word);
	}

	return NULL;
}

/**
	Rekurencyjnie zwraca rozmiar poddrzewa danego węzła.
	@param[in] n Wskaźnik na węzeł.
	@return Rozmiar poddrzewa rozpiętego przez n.
	*/

static int NodeSize(struct Node* n)

{
	if(n == NULL)
		return 0;
	
	int toReturn = 1;
	for (int i = 0; i < n->childCount; ++i)
		toReturn += NodeSize(n->children[i]);

	return toReturn;
}

///@}


/** @name Elementy interfejsu 
   @{
 */

void Tree_init(struct Tree *t)
{
	t->root = (struct Node*)malloc(sizeof(struct Node));
	Node_init(t->root);
}

void Tree_destroy(struct Tree *t)
{
	if(t->root != NULL)
		Node_destroy(t->root);
}

int Tree_add(struct Tree *t, const wchar_t* word)
{
	if(wcslen(word) == 0)
		return 0;
	if(Tree_find(t, word) == 0)
		return addNode(t->root, word);
	return 0;
}

int Tree_find(struct Tree* t, const wchar_t* word)
{
	return findNode(t->root, word);
}


int Tree_delete(struct Tree *t, const wchar_t* word)
{
	if(Tree_find(t, word) == 1)
	{
		struct Node *last = findLeaf(t->root, word);
		if((last != NULL) && (last != t->root))
		{
			while((last->parent->childCount < 2) && (last->parent != t->root))
				last = last->parent;
			
			int sonNumber = 0;

			while(last->parent->children[sonNumber] != last)
				sonNumber++;
			struct Node* toDelete = last->parent->children[sonNumber];
			
			struct Node *lastParent = last->parent;
			Node_destroy(toDelete);
			//zwolnic sam wezel toDelete - destrToLeaf to zrobi!
			
			//przesuwamy na miejsce usuwanego noda ostatni z tablicy dzieci
			if(sonNumber != lastParent->childCount - 1)
				lastParent->children[sonNumber] = 
					lastParent->children[lastParent->childCount - 1];
			lastParent->childCount--;

			qsort(lastParent->children, lastParent->childCount, 
				sizeof(struct Node*), nodeComp);
		}
		return 1;
	}
	else
		return 0;

}

int Tree_size(struct Tree* t)
{
	if(t == NULL)
		return 0;

	return NodeSize(t->root);
}

///@}



/** @name Funkcje pomocnicze do zapisywania i wczytywania.
   	@{
	*/

/**
	Dodaje do zbioru (bez powtórzeń) wszystkie znaki z kluczy dzieci zadanego
	węzła.
	@param[in] n Wskaźnik na węzeł.
	@param[in, out] s Wskaźnik na zbiór.	
	*/

static void usedInNodes(struct Node* n, struct InsertSet* s)
{
	if(n->key != '\0')
		set_add(s, n->key);
	for (int i = 0; i < n->childCount; ++i)
		usedInNodes(n->children[i], s);
}

/**
	Zwraca indeks na jeszcze nieuzupełnione miejsce na syna w zaalokowanej 
	tablicy children[] węzła z argumentu.
	@param[in] n Wskaźnik na węzeł.
	@return Indeks węzła w tablicy (@f$\geq 0@f$) jeśli istnieje, -1 wpp.
	*/

static int firstFree(struct Node* n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		if(n->children[i] == NULL)
			return i;
	}
	return -1;
}


static int checkNodeIntegrity(struct Node* n)
{
	if(firstFree(n) != -1)
		return -1;

	for (int i = 0; i < n->childCount; ++i)
		if(checkNodeIntegrity(n->children[i]) == -1)
			return -1;

	return 0;
}

///@}

/** @name Elementy interfejsu 
   @{
 */


struct InsertSet* usedInTree(struct Tree* t)
{
	struct InsertSet* toReturn =
		(struct InsertSet*)malloc(sizeof(struct InsertSet));
	set_init(toReturn);

	usedInNodes(t->root, toReturn);
	return toReturn;
}


struct Tree* Tree_load_DFS(FILE* stream)
{
	struct Tree* toReturn = (struct Tree*)(malloc(sizeof(struct Tree)));
	Tree_init(toReturn);
	int rootChildNumb; 
	wchar_t key;
	int count;

	if (fscanf(stream, "%i%lc", &rootChildNumb, &key) == EOF) 
	//najpierw wczytujemy ilu synow ma root
		return NULL;

	int error = 0;
	free(toReturn->root); //tymczasowo bo bez sensu
	toReturn->root = createNode(rootChildNumb, L'\0', NULL);
	struct Node* actual = toReturn->root; 

	while(true)
	{
		if (fscanf(stream, "%i%lc", &count, &key) == EOF)
		{
			if(checkNodeIntegrity(toReturn->root) != 0)
				error = 1;

			break;
		}
		if(key == L'!')
		{
			actual->children[firstFree(actual)] =
				createNode(count, L'\0', actual); // >\0<? spr
			while(actual != NULL)
			{
				if(firstFree(actual) != -1)
					break;

				actual = actual->parent;
			}
		}
		else
		{
			/* if(firstFree(actual) == -1) error = true; */
			int freePlace = firstFree(actual);
			actual->children[freePlace] = createNode(count, key, actual);
			//firstFree nie bedzie -1 bo dbamy o to w petli
			actual = actual->children[freePlace];
		}
	}

	if (error)
	{
		Tree_destroy(toReturn);
		free(toReturn);
		return NULL;
		//zwolnic pamiec itd, zwrocic null
	}

	return toReturn;
}

int writeDFS(struct Node* n, FILE* stream)
{
	int err = 0;
	if (n->childCount == 0)
		err = fprintf(stream, "\n%i%lc", 0, L'!');
	else
		err = fprintf(stream, "\n%i%lc", n->childCount, 
			(n->key == L'\0') ? L'\n' : n->key);

	if(err < 0)
		return err;

	if (n->childCount == 0)
		return 0;

	int toReturn = 0;
	for (int i = 0; i < n->childCount; ++i)
		toReturn += writeDFS(n->children[i], stream);
	
	return toReturn; //powinna byc suma zer w przypadku sukcesu

}

int Tree_save_DFS(struct Tree* t, FILE* stream)
{
	return writeDFS(t->root, stream);
}

///}@

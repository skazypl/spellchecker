/** @file
	Implementacja drzewa trie do przechowywania słownika.

	@ingroup dictionary
	@author Jarosław Socha <js347267@students.mimuw.edu.pl>
	*/

#include "trie.h"
#include <string.h>
#include <assert.h>
#include "io_mock.h"

/** @name Funkcje pomocnicze.
   @{
 */

/**
	Inicjalizacja węzła po jego utworzeniu.
	@param[in, out] n Węzeł drzewa.
   */

void Node_init(struct Node *n)
{
	n->key = L'\0'; //0 == '\0'
	n->parent = NULL;
	n->children = NULL;
	n->childCount = 0;
}

/**
	Tworzy węzeł o przekazanych przez argument kluczu, liczbie dzieci i rodzicu.
	@param[in] childCount Liczba dzieci i rozmiar tablicy dzieci do zaalokownia.
	@param[in] key Klucz typu wchar_t nowego węzła.
	@param[in] parent Wskaźnik do węzła rodzica.
	@return Wskaźnik do stworzonego węzła.

	*/

struct Node* createNode(int childCount, wchar_t key, struct Node* parent)
{
	struct Node* toReturn = (struct Node*)malloc(sizeof(struct Node));
	Node_init(toReturn);

	toReturn->childCount = childCount;
	toReturn->parent = parent;
	toReturn->key = key;

	toReturn->children = (struct Node**)
		malloc(sizeof(struct Node*) * childCount);

	for (int i = 0; i < childCount; ++i)
	{
		toReturn->children[i] = NULL;
	}

	return toReturn;
}

/**
	Niszczy (rekurencyjnie) węzeł i wszystkie węzły potomne.
	Zwalnia całą zaalokowaną pamięć, w tym tę przeznaczoną na węzeł.
	@param[in] n Wskaźnik do węzła.
	*/


void Node_destroy(struct Node *n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		Node_destroy(n->children[i]);
	}
	
	free(n->children);
	free(n);
}

/**
	
	*/

void destrToLeaf(struct Node *n)
{
	if(n->childCount == 1)
		destrToLeaf(n->children[0]);
	free(n);
}

/**
	Komparator na dwóch węzłach drzewa.
	Komparator ocenia relację < na podstawie ich wartości key.
	@param[in] a, b Wskaźniki do węzłów.
	@return 0 jeśli węzły mają identyczne klucze, 1 jeśli większy jest klucz
	pierwszego argumentu, -1 jeśli drugiego.

	*/

int nodeComp(const void* a, const void* b)
{
	struct Node* a_ = *(struct Node**)a;
	struct Node* b_ = *(struct Node**)b;
	if(a_->key < b_->key) return -1;
	else if(a_->key == b_->key) return 0;
	else return 1;
}

/**
	Wyszukiwanie binarne węzła o zadanym kluczu w zadanej argumentem tablicy.
	Tablica jest z założenia posortowana rosnąco 
	(w sensie komparatora @ref nodeComp).
	@param [in] arr[] Tablica
	@param [in] size Rozmiar przeszukiwanej tablicy.
	@param [in] patt Klucz, którego pociadacza poszukujemy.
	@return indeks tablicy, ktorego element jest rowny poszukiwanemu,
	-1 jesli nie ma takiego.
	*/

int binSearch(struct Node* arr[], int size, wchar_t patt)
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

int addNode(struct Node *n, const wchar_t* word)
{
	if(wcslen(word) != 0)
	{
		int ind = binSearch(n->children, n->childCount, word[0]);
		if(ind != -1)
		{
			return addNode(n->children[ind], ++word);
		}
	}

	//jezeli nie znaleziono
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

	@param[in] n Wskaźnik na węzeł.
	@param[in] word Słowo.
	@return 0 jeśli słowa nie ma w drzewie ukorzenionym w węźle, 1 jeśli jest.
	*/


int findNode(struct Node* n, const wchar_t* word)
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

struct Node* findLeaf(struct Node* n, const wchar_t* word)
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

int NodeSize(struct Node* n)

{
	int toReturn = 1;
	for (int i = 0; i < n->childCount; ++i)
		toReturn += NodeSize(n->children[i]);

	return toReturn;
}

///@}

/** @name Funkcje do debugowania.
   @{
 */

void printTree(struct Node* n, int k)
{
	for (int i = 0; i < k; ++i)
		for (int j = 0; j < 5; j++)
			printf(" ");

	printf("|>%lc<\n", n->key);
	
	for (int i = 0; i < n->childCount; ++i)
	{
		printTree(n->children[i], k+1);
	}
}

/**@}*/


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

int add(struct Tree *t, const wchar_t* word)
{
	if(wcslen(word) == 0)
		return 0;
	if(find(t, word) == 0)
		return addNode(t->root, word);
	return 0;
}

int find(struct Tree* t, const wchar_t* word)
{
	return findNode(t->root, word);
}


int delete(struct Tree *t, const wchar_t* word)
{
	if(find(t, word) == 1)
	{
		//printTree(t->root, 0);
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
	Ustawia relację <dziecko, rodzic> dla węzłów z tablicy dzieci danego węzła.
	@param[in] n Wskaźnik na węzeł.
	*/

void setParents(struct Node* n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		n->children[i]->parent = n;
		setParents(n->children[i]);
	}	
}

/**
	Dodaje do zbioru (bez powtórzeń) wszystkie znaki z kluczy dzieci zadanego
	węzła.
	@param[in] n Wskaźnik na węzeł.
	@param[in, out] s Wskaźnik na zbiór.	
	*/

void usedInNodes(struct Node* n, struct InsertSet* s)
{
	if(n->key != '\0')
		set_add(s, n->key);
	for (int i = 0; i < n->childCount; ++i)
		usedInNodes(n->children[i], s);
}

/**
	Zwraca zbiór wszystkich znaków użytych w drzewie.
	@param[in] t Wskaźnik na drzewo.
	@return wskaźnik na zbiór typu InsertSet zawierający wszystkie znaki z 
	kluczy węzłów drzewa.
	*/

struct InsertSet* usedInTree(struct Tree* t)
{
	struct InsertSet* toReturn =
		(struct InsertSet*)malloc(sizeof(struct InsertSet));
	set_init(toReturn);

	usedInNodes(t->root, toReturn);
	return toReturn;
}


/**
	Zwraca indeks na jeszcze nieuzupełnione miejsce na syna w zaalokowanej 
	tablicy children[] węzła z argumentu.
	@param[in] n Wskaźnik na węzeł.
	@return Indeks węzła w tablicy (@f$\geq 0@f$) jeśli istnieje, -1 wpp.
	*/

int firstFree(struct Node* n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		if(n->children[i] == NULL)
			return i;
	}
	return -1;
}

///@}

/** @name Elementy interfejsu 
   @{
 */



struct Tree* Tree_load_DFS(FILE* stream)
{
	struct Tree* toReturn = (struct Tree*)(malloc(sizeof(struct Tree)));
	Tree_init(toReturn);
	int scanChildNum; 
	wchar_t key;
	int count;

	if (fscanf(stream, "%i%lc", &scanChildNum, &key) == EOF) 
	//najpierw wczytujemy ilu synow ma root
		return NULL;

	int error = 0;
	free(toReturn->root); //tymczasowo bo bez sensu
	toReturn->root = createNode(scanChildNum, L'\0', NULL);
	struct Node* actual = toReturn->root; 

	while(true)
	{
		if (fscanf(stream, "%i%lc", &count, &key) == EOF)
		{
			/*
			TODO -> Dodać error handling
			zrobilbym checkTreeIntegrity() ktora sprawdza czy ost syn zadnego
			noda nie jest nullem i jezeli to nieprawda to ustawia error = 1

			if(FreeTillRoot(listTillRoot) == 0)
				error = 1; //lub zwyczajnie koniec pliku 
						//funkcja wyzej zdaje sie wyczerpywac mozliwe errory
			*/
			break;
		}

		if(key == L'!')
		{
			actual->children[firstFree(actual)] = createNode(count, L'\0', actual); // >\0<? spr
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
	{
		toReturn += writeDFS(n->children[i], stream);
	}
	return toReturn; //powinna byc suma zer w przypadku sukcesu

}

int Tree_save_DFS(struct Tree* t, FILE* stream)
{
	return writeDFS(t->root, stream);
}

///}@

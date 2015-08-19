
#include "trie.h"
#include <string.h>
#include <assert.h>

/** @name Funkcje pomocnicze.
   @{
 */

void Node_init(struct Node *n)
{
	n->key = 0; //0 == '\0'
	n->parent = NULL;
	n->children = NULL;
	n->childCount = 0;
}

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

void Node_destroy(struct Node *n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		Node_destroy(n->children[i]);
	}
	
	free(n->children);
	free(n);
}

void destrToLeaf(struct Node *n)
{
	if(n->childCount == 1)
		destrToLeaf(n->children[0]);
	free(n);
}

int nodeComp(const void* a, const void* b)
{
	struct Node* a_ = *(struct Node**)a;
	struct Node* b_ = *(struct Node**)b;
	if(a_->key < b_->key) return -1;
	else if(a_->key == b_->key) return 0;
	else return 1;
}

//zwraca indeks tablicy arr, ktorego element jest rowny patt, -1 jesli nie ma
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

void addNode(struct Node *n, const wchar_t* word)
{
	if(wcslen(word) != 0)
	{
		int ind = binSearch(n->children, n->childCount, word[0]);
		if(ind != -1)
		{
			addNode(n->children[ind], ++word);
			return;
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

	if(word[0] != '\0')
	{
		word++;
		addNode(newNode, word);
	}

}


int findNode(struct Node* n, const wchar_t* word, int prt)
{
	if(prt == 1) printTree(n, 0);

	int ind = binSearch(n->children, n->childCount, word[0]);
	if(ind != -1)
	{
		if(word[0] == L'\0')
			return 1;
		else
		{
			word++;
			return findNode(n->children[ind], word, prt);
		}
	}

	return 0;
}


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

void add(struct Tree *t, const wchar_t* word)
{
	if(find(t, word) == 0)
		addNode(t->root, word);
}

int find(struct Tree* t, const wchar_t* word)
{
	return findNode(t->root, word, 0);
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

///@}


/** @name Struktura listy.
   @{
 */


/**
	Struktura listy dwukierunkowej, przydatna przy implementacji kolejki.
	Zawiera wskaźniki do ogonów z obu swoich stron (być może NULLi), oraz,
	jako przechowywany element, wskaźnik do obiektu typu Node.
	*/
struct List
{
	struct List* next; ///< Lista poprzedzajaca.
	struct List* prev; ///< Lista następująca.
	struct Node* elem; ///< Element listy.
};

void list_init(struct List* l)
{
	l->next = NULL;
	l->prev = NULL;
	l->elem = NULL;
}

struct List* list_add(struct Node* n, struct List* l)
{
	struct List* toReturn = (struct List*)malloc(sizeof(struct List));
	toReturn->elem = n;
	toReturn->next = l;
	return toReturn;
}

void list_destroy(struct List* l)
{
	struct List* toDelete;
	while(l != NULL)
	{
		toDelete = l;
		l = l->next;
		free(toDelete);
	}
}

//@}

/** @name Funkcje pomocnicze do zapisywania i wczytywania.
   @{
 */

int treeSize(struct Node* n)

{
	int toReturn = 1;
	for (int i = 0; i < n->childCount; ++i)
		toReturn += treeSize(n->children[i]);

	return toReturn;
}

void setParents(struct Node* n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		n->children[i]->parent = n;
		setParents(n->children[i]);
	}	
}

void usedInNodes(struct Node* n, struct InsertSet* s)
{
	if(n->key != '\0')
		set_add(s, n->key);
	for (int i = 0; i < n->childCount; ++i)
		usedInNodes(n->children[i], s);
}

struct InsertSet* usedInTree(struct Tree* t)
{
	struct InsertSet* toReturn =
		(struct InsertSet*)malloc(sizeof(struct InsertSet));
	set_init(toReturn);

	usedInNodes(t->root, toReturn);
	return toReturn;
}

///@}

/** @name Elementy interfejsu 
   @{
 */


/*
algorytm wczytywania: DFS

*/


int FreeTillRoot(struct List* l) 
//sprawdza czy wierzcholki z listy az do roota wlacznie nie ma wolnych krawedzi
//lista przechowuje sciezke od pewnego node'a do roota
{
	while(l != NULL)
	{
		struct Node* n = l->elem;

		for (int i = 0; i < n->childCount; ++i)
		{
			if(n->children[i] == NULL)
				return 0;
		}

		l = l->next;
	}
	return 1;
}

int hasFree(struct Node* n)
//zakladamy ze node ma zainicjalizowaną tablicę
//moze mozna zintegrowac te funkcje z firstFree ???
{
	for (int i = 0; i < n->childCount; ++i)
	{
		if(n->children[i] == NULL)
			return 1;
	}
	return 0;
}

int firstFree(struct Node* n)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		if(n->children[i] == NULL)
			return i;
	}
	return -1;
}

struct Tree* Tree_load_DFS(FILE* stream)
{
	struct Tree* toReturn = (struct Tree*)(malloc(sizeof(struct Tree)));
	Tree_init(toReturn);
	int scanChildNum; //suma liczby dzieci z tablicy z poprzedniego kroku petli
	if (fscanf(stream, "%i", &scanChildNum) == EOF) 
	//najpierw wczytujemy ilu synow ma root
		return NULL;

	wchar_t key;
	int count;
	int error = 0;
	free(toReturn->root); //tymczasowo bo bez sensu
	toReturn->root = createNode(scanChildNum, L'\0', NULL);
	struct Node* actual = toReturn->root; //todo: dodac tablice dzieci roota scanChNr

	struct List* listTillRoot = (struct List*)malloc(sizeof(struct List));
	list_init(listTillRoot);
	free(listTillRoot); //na razie tutaj, przyda sie jak bedzie error handling

	while(true)
	{
		if (fscanf(stream, "%i%lc", &count, &key) == EOF)
		{
			/*
			TODO -> Dodać error handling

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
				if(hasFree(actual) == 1)
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
		err = fprintf(stream, "\n0!");
	
	else
		err = fprintf(stream, "\n%i%lc", n->childCount, n->key);

	if(err < 0)
		return err;

	int toReturn = 0;
	for (int i = 0; i < n->childCount; ++i)
	{
		toReturn += writeDFS(n->children[i], stream);
	}
	return toReturn; //powinna byc suma zer w przypadku sukcesu

}

int Tree_save_DFS(struct Tree* t, FILE* stream)
{
	int test = fprintf(stream, "%i", t->root->childCount);
	if(test < 0)
		return test;

	test = 0;
	for (int i = 0; i < t->root->childCount; ++i)
	{
		test += writeDFS(t->root->children[i], stream);
	}
	return test;
}

///}@

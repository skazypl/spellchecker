
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
};

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


void addNode(struct Node *n, const wchar_t* word)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		if(wcslen(word) != 0)
		{
			if( word[0] == n->children[i]->key )
			{
				addNode(n->children[i], ++word);
				return;
			}
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
	if(word[0] != '\0')
	{
		word++;
		addNode(newNode, word);
	}

}


int findNode(struct Node* n, const wchar_t* word, int prt)
{
	if(prt == 1) printTree(n, 0);

	for (int i = 0; i < n->childCount; ++i)
	{
		if(n->children[i]->key == word[0])
		{
			if(word[0] == '\0')
				return 1;
			else
			{
				word++;
				return findNode(n->children[i], word, prt);
			}
		}
	}
	return 0;
}


struct Node* findLeaf(struct Node* n, const wchar_t* word)
{
	for (int i = 0; i < n->childCount; ++i)
		if( word[0] == n->children[i]->key )
		{
			if(word[0] == '\0')
				return n->children[i];
			return findLeaf(n->children[i], ++word);
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


void delete(struct Tree *t, const wchar_t* word)
{
	if(find(t, word) == 1)
	{
		printTree(t->root, 0);
		struct Node *last = findLeaf(t->root, word);
		if((last != NULL) && (last != t->root))
		{
			while((last->parent->childCount < 2) && (last->parent != t->root))
				last = last->parent;
			
			int sonNumber = 0;

			while(last->parent->children[sonNumber] != last)
				sonNumber++;
			struct Node *toDelete = last->parent->children[sonNumber];
			
			struct Node *lastParent = last->parent;
			Node_destroy(last->parent->children[sonNumber]);
			//zwolnic sam wezel toDelete - destrToLeaf to zrobi!
			
			//free(last->parent->children[sonNumber]);

			//przesuwamy na miejsce usuwanego noda ostatni z tablicy dzieci
			if(sonNumber != lastParent->childCount - 1)
				lastParent->children[sonNumber] = 
					lastParent->children[lastParent->childCount - 1];
			lastParent->childCount--;
		}
	}
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

/** @name Struktura kolejki.
   @{
 */

/**
	Struktura kolejki przechowywującej elementy typu Node.
	Zawiera standardowe metody kolejki - konstruktor, destruktor, zdjęcie z
	początku i dodanie na koniec elementu.
	*/

struct Queue
{
	int size; ///< Rozmiar kolejki.
	struct List* first; ///< Początek kolejki.
	struct List* last; ///< Koniec kolejki.
};

void Queue_init(struct Queue* q)
{
	q->size = 0;
	q->first = (struct List*)malloc(sizeof(struct List));
	q->last = q->first;
}


void Queue_done(struct Queue* q)
{
	struct List* actual = q->first;
	while(q->first != NULL)
	{
		q->first = q->first->next;
		free(actual);
		actual = q->first;
	}
}

void Queue_push(struct Node* n, struct Queue* q)
{
	fprintf(stderr, "# size : %i\n", q->size );
	struct List* newLast;
	if(newLast = (struct List*)malloc(sizeof(struct List)))
		fprintf(stderr, "zaakolowano\n");

		fprintf(stderr, "1\n" );
	newLast->elem = n;
		fprintf(stderr, "2\n" );
	newLast->prev = q->last;
		fprintf(stderr, "3\n" );
	q->last->next = newLast;
		fprintf(stderr, "4\n" );
	q->last = q->last->next;
		fprintf(stderr, "5\n" );
	newLast->next = NULL;
		fprintf(stderr, "6\n" );

	if(q->size == 0)
		q->first = q->last;
		
		fprintf(stderr, "7\n" );	

	q->size++;
		fprintf(stderr, "8 }\n" );
}

struct Node* Queue_pop(struct Queue* q)
{
	struct Node* toReturn = q->first->elem;
	struct List* toDelete = q->first;
	q->first = q->first->next;
	free(toDelete);

	q->size--;
	return toReturn;
}

///@}

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

algorytm czytania z pliku:
1. sumuj dzieci rzeczy z aktualnej array i =: SumKids
2. alloc nowe_t[Sum]
3. for(0 < i < Sum)
       czytaj next char i int
       dodaj nowy wezel
       dowiaz do nowe_t[i]
4. lec po stare_t[] i dodawaj im synow z nowe_t[]
5.stara_t := nowe_t

*/

struct Tree* Tree_load(FILE* stream)
{
	int liczbaNodow = 1; // root
	wchar_t buf;
	int numb;
	struct Tree* toReturn = (struct Tree*)(malloc(sizeof(struct Tree)));
	int oldSum; //suma liczby dzieci z tablicy z poprzedniego kroku petli
	if (fscanf(stream, "%i", &oldSum) == EOF) 
	//najpierw wczytujemy ilu synow ma root
		return NULL;

	int sumChild = 1;
	toReturn->root = (struct Node*)(malloc(sizeof(struct Node)));
	toReturn->root->childCount = oldSum;
	toReturn->root->key = 0; //'\0'

	struct Node** nodeLineArr = 
		(struct Node**)malloc(sizeof(struct Node*));
		nodeLineArr[0] = toReturn->root;

	//czym jest nodeLineArr przy kazdorazowym wejsciu do ponizszej petli - 
	//jest tablica wskaznikow do nodow z poprzedniego kroku - nizszej glebokosc
	//przy 1 wejsciu jest tylko zlozona z roota
	while (sumChild != 0)
	{
		oldSum = sumChild;
		sumChild = 0;
		for (int i = 0; i < oldSum; ++i)
			sumChild +=	nodeLineArr[i]->childCount;
		//1
		struct Node** newNodeArr = 
			(struct Node**)malloc(sumChild * sizeof(struct Node**)); //2


		for (int i = 0; i < sumChild; ++i)
		{
			if(fscanf(stream, "%lc %i", &buf, &numb) == EOF)
			{
				printf("ERROR: bledny plik\n");
				free(toReturn);
				return NULL;
			}

			struct Node* newNode = (struct Node*)(malloc(sizeof(struct Node)));
			liczbaNodow++;
			if(buf == L'!')
			{
				newNode->key = 0; // '\0'
				newNode->childCount = 0;
			}
			else
			{
				newNode->key = buf;
				newNode->childCount = numb;
			}
			newNodeArr[i] = newNode;
		}//3

		int index = 0;
		for (int i = 0; i < oldSum; ++i)
		{
			nodeLineArr[i]->children = (struct Node**)
				malloc(nodeLineArr[i]->childCount * sizeof(struct Node*));

			for (int j = 0; j < nodeLineArr[i]->childCount; ++j)
			{
				nodeLineArr[i]->children[j] = newNodeArr[index + j];
			}
			
			index += nodeLineArr[i]->childCount;
		}//4

		free(nodeLineArr);
		nodeLineArr = newNodeArr; //5
	}
	free(nodeLineArr);

	setParents(toReturn->root);
	return toReturn;
}




int Tree_save(struct Tree* t, FILE* stream)
{
	int liczbaNodow = 1; // root
	fprintf(stream, "%i", t->root->childCount);

	struct Queue nodeQueue;
	struct Queue* Q = &nodeQueue;
	Queue_init(Q);
	struct Node* n;
	printf("lecimy z drzewaem\n");
	Queue_push(t->root, Q);
	while(Q->size > 0)
	{
		fprintf(stderr, ":: petla\n" );
		n = Queue_pop(Q);
		fprintf(stderr, "bump >%lc<\n", n->key); //dbg
		for (int i = 0; i < n->childCount; ++i)
		{
				if(n->key == L's') fprintf(stderr, "petl 1\n");
			liczbaNodow++;
				if(n->key == L's') fprintf(stderr, "petl 2\n");
			if(n->children[i]->key == '\0')
				fprintf(stream, "!0");
			else
				fprintf(stream, "%lc\n%i", n->children[i]->key, 
					n->children[i]->childCount);
				//patrz konwencja
			
				if(n->key == L's') 
				{
					fprintf(stderr, "petl 3 >%lc< rozm: \n",
					 n->children[i]->key);
					fprintf(stderr, "stil\n");
				}

			Queue_push(n->children[i], Q);
				if(n->key == L's') fprintf(stderr, "petl 4\n");
		}
		fprintf(stderr, "doszlo ::\n" );
		
	}
	fprintf(stderr, "koniec kolejki\n");
	Queue_done(Q);

	return 0;
}

/*
algorytm wczytywania:

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
	int liczbaNodow = 1; // root
	wchar_t buf;
	int numb;
	struct Tree* toReturn = (struct Tree*)(malloc(sizeof(struct Tree)));
	Tree_init(toReturn);
	int scanChildNum; //suma liczby dzieci z tablicy z poprzedniego kroku petli
	if (fscanf(stream, "%i", &scanChildNum) == EOF) 
	//najpierw wczytujemy ilu synow ma root
		return NULL;

	wchar_t key;
	int count;
	int error = 0;
	struct Node* actual = t->root; //todo: dodac tablice dzieci roota scanChNr
	struct List* listTillRoot;
	list_init(listTillRoot);

	while(true)
	{
		if (fscanf(stream, "%lc %i", key, count) == EOF)))
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
			actual->children[0] = createNode(count, L'\0', actual); // >\0<? spr
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

void writeDFS(struct Node* n, FILE* stream)
{
	if (n->childCount == 0)
		fprintf(stream, "!\n0");
	else
		fprintf(stream, "%lc\n%i", n->key, n->childCount);

	for (int i = 0; i < n->childCount; ++i)
	{
		writeDFS(n->children[i], stream);
	}

}

int Tree_save_DFS(struct Tree* t, FILE* stream)
{
	fprintf(stream, "%i", t->root->childCount);

	for (int i = 0; i < t->root->childCount; ++i)
	{
		writeDFS(t->root->children[i], stream);
	}
	return 0; //jeszcze ma zwracac <0 jak cos sie nie powiedzie - TODO
}

///}@

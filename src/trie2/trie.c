
#include "trie.h"
#include <string.h>

/*
	struct Node
	{
		//int ifWord; //1 - slowo; 0 - nie
		//lepsza konwencja: pusty wezel tzn '\0'

		char key;
		struct Node* parent;
		struct Node* children[MAX_SONS]; // wolalbym sons
		//todo: synowie alfabetycznie
		size_t childCount;
	};

	struct Tree
	{
		Node* root;
	};

*/

//todo: trzymac w lisciach wysokosc - pomoze w load()

int startsWith(const char* haystack, const char* needle) //0 - nie, 1 - tak
//szukamy czy tekst igly rozpoczyna tekst stoguSiana
{
	if( (haystack == NULL) && (needle == NULL) ) return 1;
	if(haystack == NULL) return 0;
	if(needle == NULL) return 1;

	if(LENG(haystack) < LENG(needle))
		return 0;

	char* j = haystack;
	for (char* i = needle; *i != '\0'; ++i)
	{
		if(*i != *j)
			return 0;
		j++;
	}
	return 1;
}

void Node_init(struct Node *n)
{
	n->key = 0; //0 == '\0'
	n->parent = NULL;
	n->childCount = 0;
};

/// root bedzie wezlem atrapa - nie bedziemy przechowywac w nim stringa
void Tree_init(struct Tree *t)
{
	t->root = (struct Node*)malloc(sizeof(struct Node));
	Node_init(t->root);
}


void Node_destroy(struct Node *n)
{
	for (int i = 0; i < n->childCount; ++i)
		Node_destroy(n->children[i]);
	
	//free(n->children);
	free(n);
}

void Tree_destroy(struct Tree *t)
{
	if(t->root != NULL)
		Node_destroy(t->root);
}


void addNode(struct Node *n, char* word)
{
	//todo: co zakladamy wchodzac do wezla?
	//1. ze nie jest null

	//todo2: podwojnie dodaje pusty wezel przy wstawianiu 
	//dwa razy tego samego slowa - usunac usterke

	printf("word[0]: %c\n", word[0]);

	for (int i = 0; i < n->childCount; ++i)
	{
		printf("n->children[i]->key: >%c<\n", n->children[i]->key);
		if( word[0] == n->children[i]->key )
		{
			addNode(n->children[i], ++word);
			return;
		}
	}

	//jezeli nie znaleziono
	n->childCount++;
	struct Node* newNode = (struct Node*)(malloc(sizeof(struct Node)));
	n->children[n->childCount - 1] = newNode;
	Node_init(newNode);
	newNode->parent = n;
	newNode->key = word[0];

	if(*word != '\0')
	{
		word++;
		addNode(newNode, word);
	}

}


void add(struct Tree *t, char* word)
{
	if(find(t, word) == 0) //?? zmniejsza efektywnosc
		addNode(t->root, word);

}


//debug
void printTree(struct Node* n, int k)
{
	for (int i = 0; i < k; ++i)
		for (int j = 0; j < 5; j++)
			printf(" ");

	printf("|>%c<\n", n->key);
	
	for (int i = 0; i < n->childCount; ++i)
	{
		printTree(n->children[i], k+1);
	}
}


int findNode(struct Node* n, char* word)
{
	for (int i = 0; i < n->childCount; ++i)
	{
		if(n->children[i]->key == word[0])
		{
			if(word[0] == '\0')
				return 1;
			else
			{
				word++;
				return findNode(n->children[i], word);
			}
		}
	}
	return 0;
}


int find(struct Tree* t, char* word) //1 - slowo jest w slowniku; 0 - nie ma
{
	return findNode(t->root, word);
}


struct Queue
{
	int size;
	struct Node* *content; //tablica
};

void Queue_init(struct Queue* q)
{
	q->size = 0;
	q->content = NULL;
}

void Queue_push(struct Node* n, struct Queue* q)
{
	struct Node** newContent = (struct Node**)malloc((q->size + 1) * sizeof(struct Node*));
	for (int i = 0; i < q->size; ++i)
		newContent[i] = q->content[i];
	newContent[q->size] = n;
	q->size++;
	free(q->content); //nie no to trzeba zwalniac
	q->content = newContent;
}

struct Node* Queue_pop(struct Queue* q) //!przed wywolaniem sprawdzac size > 0
{
	struct Node* toReturn = q->content[0];
	struct Node** newContent = (struct Node**)malloc(q->size * sizeof(struct Node*));
	for (int i = 1; i < q->size; ++i)
		newContent[i-1] = q->content[i];
	q->size--;
	free(q->content);
	q->content = newContent;
	return toReturn;
}


struct Tree* Tree_load(FILE* stream)
{

	wchar_t* buf;
	int* numb;
	struct Tree* toReturn = (struct Tree*)(malloc(sizeof(struct Tree)));
	int sumChild = -1;
	struct Node** nodeLineArr = &(toReturn->root); //tablica wskaznikow
	int arrSize = 1;
	while (sumChild != 0)
	{
		int oldSum = sumChild;
		sumChild = 0;
		for (int i = 0; i < arrSize; ++i)
		{
			sumChild +=	nodeLineArr[i]->childCount;
		}//1

		struct Node** newNodeArr[sumChild]; //2

		for (int i = 0; i < sumChild; ++i)
		{
			if(fscanf(stream, "%32ls%i", buf, numb) == EOF)
			{
				printf("ERROR: bledny plik\n");
				return NULL;
			}

			struct Node* newNode = (struct Node*)(malloc(sizeof(struct Node)));
			if(buf == '!')
			{
				newNode->key = 0; // '\0'
				newNode->childCount = 0;
			}
			else
			{
				newNode->key = *buf;
				newNode->childCount = *numb;
			}
			newNodeArr[i] = newNode;
		}//3

		int index = 0;
		for (int i = 0; i < oldSum; ++i)
		{
			for (int j = 0; j < nodeLineArr[i]->childCount; ++j)
				nodeLineArr[i]->children[j] = newNodeArr[index + j];
			
			index += nodeLineArr[i]->childCount;
		}//4
		
		nodeLineArr = newNodeArr; //5
	}
	return toReturn;
}
/*

algo:
1. sumuj dzieci rzeczy z aktualnej array i =: SumKids
2. alloc nowe_t[Sum]
3. for(0 < i < Sum)
       czytaj next char i int
       dodaj nowy wezel
       dowiaz do nowe_t[i]
4. lec po stare_t[] i dodawaj im synow z nowe_t[]
5.stara_t := nowe_t

*/




int Tree_save(struct Tree* t, FILE* stream)
{
	struct Queue nodeQueue;
	struct Queue* Q = &nodeQueue;
	Queue_init(Q);
	struct Node* n;

	Queue_push(t->root, Q);
	while(Q->size > 0)
	{
		n = Queue_pop(Q);
		for (int i = 0; i < n->childCount; ++i)
		{
			if(n->children[i]->key == '\0')
				fprintf(stream, "!0");
			else
				fprintf(stream, "%c%i", n->children[i]->key, n->children[i]->childCount);
				//patrz konwencja
			
			Queue_push(n->children[i], Q);
		}
	}
}




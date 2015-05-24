
#include "trie.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
}a

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



int find(struct Tree* t, char* word) //1 - slowo jest w slowniku; 0 - nie ma
{
	return findNode(t->root);
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

struct Tree Tree_load(FILE* stream);
int Tree_save(struct Tree* t, FILE* stream);


#include "trie.h"
#include <string.h>
#include <stdlib.h>

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
	n->parent = NULL;
	n->childCount = 0;
}

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
	
	free(n->children);
}

void Tree_destroy(struct Tree *t)
{
	if(t->root != NULL)
		Node_destroy(t->root);
}

void add(struct Tree *t, char* word)
{
	//if(find(t, word) == 1)) return; //?? zmniejsza efektywnosc
	addNode(t->root, word);

}

void addNode(struct Node *n, char* word)
{
	//todo: co zakladamy wchodzac do wezla?
	//1. ze nie jest null

	if(*word == '\0') return;

	for (int i = 0; i < n->childCount; ++i)
	{
		if( word[0] == n->key )
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
	//newNode->key

}




int find(struct Tree *t, char* word); //1 - slowo jest w slowniku; 0 - nie ma

size_t sizeOf(struct Tree *t);

char* getWords(struct Tree *t);
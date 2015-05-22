
#include "trie.h"
#include <string.h>

/*
	struct Node
	{
		//int ifWord; //1 - slowo; 0 - nie
		//lepsza konwencja: pusty wezel tzn '\0'

		char* string;
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
	if( (haystack == NULL) && (needle == NULL)) return 1;
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


void Tree_init(struct Tree *t)
{
	t->root = NULL;
}

void Node_init(struct Node *n)
{
	n->string = NULL;
	n->parent = NULL;
	n->children = NULL;
	n->childCount = 0;
}

void Tree_destroy(struct Tree *t)
{
	if(t->root != NULL)
		Node_destroy(t->root);
}

void Node_destroy(struct Node *n)
{
	for (int i = 0; i < childCount; ++i)
		Node_destroy(children[i]);
	
	free(n->string);
	free(n->children);
}

void add(struct Tree *t);
int find(struct Tree *t); //1 - slowo jest w slowniku; 0 - nie ma

size_t sizeOf(struct Tree *t);

char* getWords(struct Tree *t);
#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

//typedef int size_t; //kompilator szwankuje

#define MAX_SONS 26
#define LENG(x) (sizeof(x) / sizeof(x[0])) //sprawdzac tylko dla niepustej!

	struct Node
	{
		//int ifWord; //1 - slowo; 0 - nie
		//lepsza konwencja: pusty wezel tzn '\0'

		wchar_t key;
		struct Node* parent;
		struct Node* children[MAX_SONS]; // wolalbym sons
		//todo: synowie alfabetycznie
		int childCount;
	};

	struct Tree
	{
		struct Node* root;
	};

	void Tree_init(struct Tree *t);
	void Tree_destroy(struct Tree *t);

	void add(struct Tree *t, const wchar_t* word);
	int find(struct Tree *t, const wchar_t* word); //1 - slowo jest w slowniku; 0 - nie ma

	struct Tree* Tree_load(FILE* stream);
	int Tree_save(struct Tree* t, FILE* stream);
	/*
	konwencja przechowywania drzewa na dysku: ciag par
	
	<wchar_t - klucz node'a><int - liczba potomkow>
	
	dla liscia: "!0"

	mozna czytac za pomoca scanfa i "%c%i"

	*/

	void printTree(struct Node* n, int k);




#endif
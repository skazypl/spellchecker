#include <stdio.h>
#include "trie.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
	char matka[] = "matka";
	char matkara[] = "matkara";
	char marmite[] = "marmite";
	char marmolada[] = "marmolada";
	char inneSlowo[] = "inneSlowo";

	struct Tree drz;
	
	struct Tree* drzewo = &drz;
	Tree_init(drzewo);

	add(drzewo, matka);
	printTree(drzewo->root, 0);

	add(drzewo, matka);
	printTree(drzewo->root, 0);

	add(drzewo, marmolada);
	printTree(drzewo->root, 0);
	
	printf("slowo %s: %i\n", matka, find(drzewo, matka));
	printf("slowo %s: %i\n", marmolada, find(drzewo, marmolada));
	printf("slowo %s: %i\n", matkara, find(drzewo, matkara));

	FILE* fileToSave;
	fileToSave = fopen("z_BFS.txt", "w");

	Tree_save(drzewo, fileToSave);
	if(fileToSave == NULL)
		printf("ERR: nie udalo sie otworzyc pliku to_save\n");
	fclose(fileToSave);

	
	FILE* fileToLoad;
	fileToLoad = fopen("z_BFS.txt", "r");
	struct Tree* drzewo2 = Tree_load(fileToLoad);
	fclose(fileToLoad);

	if(drzewo2 != NULL)
		printTree(drzewo2->root, 0);
	else
		printf("null err\n");

	/*
	add(drzewo, matkara);
	printTree(drzewo->root, 0);


	add(drzewo, marmite);
	printTree(drzewo->root, 0);

	add(drzewo, marmolada);
	printTree(drzewo->root, 0);

	add(drzewo, inneSlowo);
	printTree(drzewo->root, 0);
	*/

	printf("\nfin\n");
	Tree_destroy(drzewo);
	Tree_destroy(drzewo2);	


	return 0;
}

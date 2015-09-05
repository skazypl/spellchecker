/**
	@file
	Testy zapisu i odczytu drzewa z pliku podmieniające funkcje I/O na atrapy
	
	@ingroup dictionary
	@author Jarosław Socha <js347267@students.mimuw.edu.pl>	
*/
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <cmocka.h>
#include <stdio.h>
#include "trie.h"

#define MAX_BUFF 10000

/*
	Bufor na który przekierujemy strumienie czytania i pisania do pliku -
	patrz io_mock.h
*/
static wchar_t wcBuff[MAX_BUFF];
static int iBuff[MAX_BUFF];

static int wcSize, iSize;

// Funkcje - zaślepki
int new_fprintf(FILE* stream, const char *format, int i, wchar_t wc)
{
	if(wc == L'\n')
		wc = L'\0';
		
	wcBuff[wcSize] = wc;
	wcSize++;
	wcBuff[wcSize] = EOF;

	iBuff[iSize] = i;
	iSize++;
	iBuff[iSize] = EOF;
	return 1;
}

static int wcActual, iActual;

int new_fscanf(FILE* stream, const char *format, int* i, wchar_t* wc)
{
	if((wcBuff[wcActual] == -1) || (iBuff[iActual] == -1))
		return -1;

	*wc = wcBuff[wcActual];   

	wcActual++;
	
	*i = iBuff[iActual];
	iActual++;

	return 1;
}

const wchar_t* first  =  L"masą";
const wchar_t* second =  L"ma";
const wchar_t* third  =  L"mat";
const wchar_t* forth  =  L"noga";
const wchar_t* fifth  =  L"noś";
const wchar_t* sixth  =  L"nota";

/*
	Test samego bufora
*/
static void buffer_test(void **state) {
	struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));

	Tree_init(t);
	assert_non_null(t);
	assert_int_equal(Tree_size(t), 1);
	assert_int_equal(Tree_add(t, first), 1);
	assert_int_equal(Tree_size(t), 6);
	assert_int_equal(Tree_add(t, third), 1);
	assert_int_equal(Tree_size(t), 8);

	wcSize = 0;
	iSize = 0;
	assert_int_equal(Tree_save_DFS(t, stderr), 0);

	assert_int_equal(wcBuff[wcSize], -1);
	assert_int_equal(wcSize, 8);

	#define check_wcBuf(x,y) assert_int_equal(wcBuff[x],y)
	check_wcBuf(0, L'\0');    
	check_wcBuf(1, L'm');
	check_wcBuf(2, L'a');
	check_wcBuf(3, L's');
	check_wcBuf(4, L'ą');
	check_wcBuf(5, L'!');
	check_wcBuf(6, L't');
	check_wcBuf(7, L'!');
	check_wcBuf(8, -1); //-1 == EOF
	#undef check_wcBuf

	#define check_iBuf(x,y) assert_int_equal(iBuff[x],y)
	check_iBuf(0, 1);
	check_iBuf(1, 1);
	check_iBuf(2, 2);
	check_iBuf(3, 1);
	check_iBuf(4, 1);
	check_iBuf(5, 0);
	check_iBuf(6, 1);
	check_iBuf(7, 0);
	check_iBuf(8, -1);
	#undef check_iBuf

	Tree_destroy(t);
	free(t);

}

/*
	Test inicjalizacji drzewa i zapisania do pliku a następnie wczytania
*/
static void trie_init_test(void **state) {
	struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));
	Tree_init(t);
	assert_non_null(t);
	assert_int_equal(Tree_size(t), 1);
	assert_int_equal(Tree_add(t, first), 1);
	assert_int_equal(Tree_size(t), 6);
	assert_int_equal(Tree_add(t, second), 1);
	assert_int_equal(Tree_size(t), 7);
	assert_int_equal(Tree_add(t, third), 1);
	assert_int_equal(Tree_size(t), 9);
	assert_int_equal(Tree_add(t, forth), 1);
	assert_int_equal(Tree_size(t), 14);
	assert_int_equal(Tree_add(t, fifth) , 1);
	assert_int_equal(Tree_size(t), 16);
	assert_int_equal(Tree_add(t, sixth), 1);
	assert_int_equal(Tree_size(t), 19);

	wcSize = 0;
	iSize = 0;
	assert_int_equal(Tree_save_DFS(t, stderr), 0);
	Tree_destroy(t);
	free(t);

	wcActual = 0;
	iActual = 0;
	struct Tree* t2 = Tree_load_DFS(stderr);
	assert_non_null(t2);

	assert_int_equal(Tree_size(t2), 19);
	assert_int_equal(Tree_find(t2, second), 1);
	assert_int_equal(Tree_find(t2, second), 1);
	assert_int_equal(Tree_find(t2, third), 1);
	assert_int_equal(Tree_find(t2, forth), 1);
	assert_int_equal(Tree_find(t2, fifth) , 1);
	assert_int_equal(Tree_find(t2, sixth), 1);

	Tree_destroy(t2);
	free(t2);
}

/*
	Test dodawania słów do wczytanego już drzewa
*/
static void trie_add_test(void **state) {
	struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));	

	Tree_init(t);
	assert_non_null(t);
	assert_int_equal(Tree_size(t), 1);
	assert_int_equal(Tree_add(t, first), 1);
	assert_int_equal(Tree_size(t), 6);
	assert_int_equal(Tree_add(t, second), 1);
	assert_int_equal(Tree_size(t), 7);
	assert_int_equal(Tree_add(t, third), 1);
	assert_int_equal(Tree_size(t), 9);
	assert_int_equal(Tree_add(t, forth), 1);
	assert_int_equal(Tree_size(t), 14);
	assert_int_equal(Tree_add(t, fifth) , 1);
	assert_int_equal(Tree_size(t), 16);
	assert_int_equal(Tree_add(t, sixth), 1);
	assert_int_equal(Tree_size(t), 19);
	
	wcSize = 0;
	iSize = 0;
	assert_int_equal(Tree_save_DFS(t, stderr), 0);
	Tree_destroy(t);
	free(t);

	wcActual = 0;
	iActual = 0;
	struct Tree* t2 = Tree_load_DFS(stderr);

	assert_int_equal(Tree_add(t2, first), 0);
	assert_int_equal(Tree_add(t2, second), 0);
	assert_int_equal(Tree_add(t2, third), 0);
	assert_int_equal(Tree_add(t2, forth), 0);
	assert_int_equal(Tree_add(t2, fifth), 0);
	assert_int_equal(Tree_add(t2, sixth), 0);

	assert_int_equal(Tree_add(t2, L""), 0);
	assert_int_equal(Tree_add(t2, L"x"), 1);
	assert_int_equal(t2->root->childCount, 3);
	assert_int_equal(Tree_add(t2, L"mi"), 1);
	assert_int_equal(Tree_add(t2, L"mas"), 1);
	assert_int_equal(Tree_add(t2, L"max"), 1);
	assert_int_equal(Tree_add(t2, L"masy"), 1);
	assert_int_equal(Tree_add(t2, L"mata"), 1);
	assert_int_equal(Tree_add(t2, L"masąc"), 1);
	assert_int_equal(Tree_add(t2, L"nie"), 1);
	assert_int_equal(Tree_add(t2, L"no"), 1);
	assert_int_equal(Tree_add(t2, L"na"), 1);
	assert_int_equal(Tree_add(t2, L"nog"), 1);
	assert_int_equal(Tree_add(t2, L"not"), 1); 
	assert_int_equal(Tree_add(t2, L"nora"), 1);
	assert_int_equal(Tree_add(t2, L"notaa"), 1);
	assert_int_equal(Tree_add(t2, L"nośnik"), 1);
	assert_int_equal(Tree_add(t2, L"nogami"), 1);
	assert_int_equal(Tree_add(t2, L"notatka"), 1);

	const wchar_t* veryLong = 
		L"verylongwordnotintreewithpolishcharsxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxqwertyuiopąęćńźżół";
	
	assert_int_equal(Tree_add(t2, veryLong), 1);

	Tree_destroy(t2);
	free(t2);
}

/*
	Test samego niszczenia drzewa wczytanego ze strumienia
*/
static void trie_destroy_test(void **state) {

	wcActual = 0;
	iActual = 0;
	struct Tree* t = Tree_load_DFS(stderr);
	Tree_destroy(t);
	free(t);
}

/*
	Test wyszukiwania we wczytanym drzewie
*/
static void trie_find_test(void** state) {
	struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));	

	Tree_init(t);
	assert_non_null(t);
	assert_int_equal(Tree_size(t), 1);
	assert_int_equal(Tree_add(t, first), 1);
	assert_int_equal(Tree_size(t), 6);
	assert_int_equal(Tree_add(t, second), 1);
	assert_int_equal(Tree_size(t), 7);
	assert_int_equal(Tree_add(t, third), 1);
	assert_int_equal(Tree_size(t), 9);
	assert_int_equal(Tree_add(t, forth), 1);
	assert_int_equal(Tree_size(t), 14);
	assert_int_equal(Tree_add(t, fifth) , 1);
	assert_int_equal(Tree_size(t), 16);
	assert_int_equal(Tree_add(t, sixth), 1);
	assert_int_equal(Tree_size(t), 19);
	
	wcSize = 0;
	iSize = 0;
	assert_int_equal(Tree_save_DFS(t, stderr), 0);
	Tree_destroy(t);
	free(t);

	wcActual = 0;
	iActual = 0;
	struct Tree* t2 = Tree_load_DFS(stderr);
	assert_int_equal(Tree_find(t2, first), 1);
	assert_int_equal(Tree_find(t2, second), 1);
	assert_int_equal(Tree_find(t2, third), 1);
	assert_int_equal(Tree_find(t2, forth), 1);
	assert_int_equal(Tree_find(t2, fifth), 1);
	assert_int_equal(Tree_find(t2, sixth), 1);

	assert_int_equal(Tree_find(t2, L""), 0);
	assert_int_equal(Tree_find(t2, L"x"), 0);
	assert_int_equal(Tree_find(t2, L"mi"), 0);
	assert_int_equal(Tree_find(t2, L"mas"), 0);
	assert_int_equal(Tree_find(t2, L"max"), 0);
	assert_int_equal(Tree_find(t2, L"masy"), 0);
	assert_int_equal(Tree_find(t2, L"mata"), 0);
	assert_int_equal(Tree_find(t2, L"masąc"), 0);
	assert_int_equal(Tree_find(t2, L"nie"), 0);
	assert_int_equal(Tree_find(t2, L"no"), 0);
	assert_int_equal(Tree_find(t2, L"na"), 0);
	assert_int_equal(Tree_find(t2, L"nog"), 0);
	assert_int_equal(Tree_find(t2, L"not"), 0);    
	assert_int_equal(Tree_find(t2, L"nora"), 0);
	assert_int_equal(Tree_find(t2, L"notaa"), 0);
	assert_int_equal(Tree_find(t2, L"nośnik"), 0);
	assert_int_equal(Tree_find(t2, L"nogami"), 0);
	assert_int_equal(Tree_find(t2, L"notatka"), 0);

	const wchar_t* veryLong = 
		L"verylongwordnotintreewithpolishcharsxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
		xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxqwertyuiopąęćńźżół";
	
	assert_int_equal(Tree_find(t2, veryLong), 0);
	assert_int_equal(Tree_add(t2, veryLong), 1);
	assert_int_equal(Tree_find(t2, veryLong), 1);
	
	Tree_destroy(t2);
	free(t2);
}

/*
	Test usuwania z wczytanego drzewa
*/
static void trie_delete_test(void** state) {
	struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));	

	Tree_init(t);
	assert_non_null(t);
	assert_int_equal(Tree_size(t), 1);
	assert_int_equal(Tree_add(t, first), 1);
	assert_int_equal(Tree_size(t), 6);
	assert_int_equal(Tree_add(t, second), 1);
	assert_int_equal(Tree_size(t), 7);
	assert_int_equal(Tree_add(t, third), 1);
	assert_int_equal(Tree_size(t), 9);
	assert_int_equal(Tree_add(t, forth), 1);
	assert_int_equal(Tree_size(t), 14);
	assert_int_equal(Tree_add(t, fifth) , 1);
	assert_int_equal(Tree_size(t), 16);
	assert_int_equal(Tree_add(t, sixth), 1);
	assert_int_equal(Tree_size(t), 19);
	
	wcSize = 0;
	iSize = 0;
	assert_int_equal(Tree_save_DFS(t, stderr), 0);
	Tree_destroy(t);
	free(t);

	wcActual = 0;
	iActual = 0;
	struct Tree* t2 = Tree_load_DFS(stderr);

	assert_int_equal(Tree_delete(t2, first), 1);
	assert_int_equal(Tree_find(t2, first), 0);
	assert_int_equal(Tree_add(t2, first), 1);
	assert_int_equal(Tree_find(t2, first), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, second), 1);
	assert_int_equal(Tree_find(t2, second), 0);
	assert_int_equal(Tree_add(t2, second), 1);
	assert_int_equal(Tree_find(t2, second), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, third), 1);
	assert_int_equal(Tree_find(t2, third), 0);
	assert_int_equal(Tree_add(t2, third), 1);
	assert_int_equal(Tree_find(t2, third), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, forth), 1);
	assert_int_equal(Tree_find(t2, forth), 0);
	assert_int_equal(Tree_add(t2, forth), 1);
	assert_int_equal(Tree_find(t2, forth), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, fifth), 1);
	assert_int_equal(Tree_find(t2, fifth), 0);
	assert_int_equal(Tree_add(t2, fifth), 1);
	assert_int_equal(Tree_find(t2, fifth), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, sixth), 1);
	assert_int_equal(Tree_find(t2, sixth), 0);
	assert_int_equal(Tree_add(t2, sixth), 1);
	assert_int_equal(Tree_find(t2, sixth), 1);
	assert_int_equal(Tree_size(t2), 19);


	/* testy podwójnego usuwania */
	assert_int_equal(Tree_delete(t2, first), 1);
	assert_int_equal(Tree_find(t2, first), 0);
	assert_int_equal(Tree_delete(t2, first), 0);
	assert_int_equal(Tree_find(t2, first), 0);
	assert_int_equal(Tree_add(t2, first), 1);
	assert_int_equal(Tree_find(t2, first), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, second), 1);
	assert_int_equal(Tree_find(t2, second), 0);
	assert_int_equal(Tree_delete(t2, second), 0);
	assert_int_equal(Tree_find(t2, second), 0);
	assert_int_equal(Tree_add(t2, second), 1);
	assert_int_equal(Tree_find(t2, second), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, third), 1);
	assert_int_equal(Tree_find(t2, third), 0);
	assert_int_equal(Tree_delete(t2, third), 0);
	assert_int_equal(Tree_find(t2, third), 0);
	assert_int_equal(Tree_add(t2, third), 1);
	assert_int_equal(Tree_find(t2, third), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, forth), 1);
	assert_int_equal(Tree_find(t2, forth), 0);
	assert_int_equal(Tree_delete(t2, forth), 0);
	assert_int_equal(Tree_find(t2, forth), 0);
	assert_int_equal(Tree_add(t2, forth), 1);
	assert_int_equal(Tree_find(t2, forth), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, fifth), 1);
	assert_int_equal(Tree_find(t2, fifth), 0);
	assert_int_equal(Tree_delete(t2, fifth), 0);
	assert_int_equal(Tree_find(t2, fifth), 0);
	assert_int_equal(Tree_add(t2, fifth), 1);
	assert_int_equal(Tree_find(t2, fifth), 1);
	assert_int_equal(Tree_size(t2), 19);

	assert_int_equal(Tree_delete(t2, sixth), 1);
	assert_int_equal(Tree_find(t2, sixth), 0);
	assert_int_equal(Tree_delete(t2, sixth), 0);
	assert_int_equal(Tree_find(t2, sixth), 0);
	assert_int_equal(Tree_add(t2, sixth), 1);
	assert_int_equal(Tree_find(t2, sixth), 1);
	assert_int_equal(Tree_size(t2), 19);    

	//--------
	assert_int_equal(Tree_delete(t2, L""), 0);
	assert_int_equal(Tree_delete(t2, L"x"), 0);
	assert_int_equal(Tree_delete(t2, L"mi"), 0);
	assert_int_equal(Tree_delete(t2, L"mas"), 0);
	assert_int_equal(Tree_delete(t2, L"max"), 0);
	assert_int_equal(Tree_delete(t2, L"masy"), 0);
	assert_int_equal(Tree_delete(t2, L"mata"), 0);
	assert_int_equal(Tree_delete(t2, L"masąc"), 0);
	assert_int_equal(Tree_delete(t2, L"nie"), 0);
	assert_int_equal(Tree_delete(t2, L"no"), 0);
	assert_int_equal(Tree_delete(t2, L"na"), 0);
	assert_int_equal(Tree_delete(t2, L"nog"), 0);
	assert_int_equal(Tree_delete(t2, L"not"), 0);    
	assert_int_equal(Tree_delete(t2, L"nora"), 0);
	assert_int_equal(Tree_delete(t2, L"notaa"), 0);
	assert_int_equal(Tree_delete(t2, L"nośnik"), 0);
	assert_int_equal(Tree_delete(t2, L"nogami"), 0);
	assert_int_equal(Tree_delete(t2, L"notatka"), 0);

	Tree_destroy(t2);
	free(t2);
}

#define run_trie_test(x) cmocka_unit_test_setup_teardown(x, trie_setup, \
	trie_teardown)

int main(void) {
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(buffer_test),
		cmocka_unit_test(trie_init_test),
		cmocka_unit_test(trie_destroy_test),
		cmocka_unit_test(trie_add_test),
		cmocka_unit_test(trie_find_test),
		cmocka_unit_test(trie_delete_test)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
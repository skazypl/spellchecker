/**
	@file
	Testy funkcji interfejsu dictionary.
		
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
#include "dictionary.h"

const wchar_t* first  =  L"ax"; ///< Wstawiany string.
const wchar_t* second =  L"ay"; ///< Wstawiany string.
const wchar_t* third  =  L"b"; ///< Wstawiany string.
const wchar_t* forth  =  L"c"; ///< Wstawiany string.
const wchar_t* fifth  =  L"cpq"; ///< Wstawiany string.

const wchar_t* firstSimilar  =  L"aa"; ///< Wstawiany string.
const wchar_t* secondSimilar =  L"ab"; ///< Wstawiany string.
const wchar_t* thirdSimilar  =  L"b"; ///< Wstawiany string.
const wchar_t* forthSimilar  =  L"c"; ///< Wstawiany string.
const wchar_t* fifthSimilar  =  L"cab"; ///< Wstawiany string.

/// Funkcja sprawdzająca inicjalizację i od razu destrukcję słownika.
static void dict_new_done_test(void **state) {
	struct dictionary* d = dictionary_new();
	assert_non_null(d);
	dictionary_done(d);
}

/// Funkcja sprawdzająca inicjalizację, dodanie słów ze stałych i destrukcję.
static void dict_setup_done_test(void **state) {
	struct dictionary* d = dictionary_new();
	assert_non_null(d);
	assert_int_equal(dictionary_insert(d, first), 1);
	assert_int_equal(dictionary_insert(d, second), 1);
	assert_int_equal(dictionary_insert(d, third), 1);
	assert_int_equal(dictionary_insert(d, forth), 1);
	assert_int_equal(dictionary_insert(d, fifth), 1);
	dictionary_done(d);
}

/// Ustawia słownik do testów setup_teardown.b
static int dict_setup(void **state) {
	struct dictionary* d = dictionary_new();
	if(!d)
		return -1;
	assert_int_equal(dictionary_insert(d, first), 1);
	assert_int_equal(dictionary_insert(d, second), 1);
	assert_int_equal(dictionary_insert(d, third), 1);
	assert_int_equal(dictionary_insert(d, forth), 1);
	assert_int_equal(dictionary_insert(d, fifth), 1);
	*state = d;
	return 0;
}

/// Ustawia słownik z podobnych stringów do testów setup_teardown.
static int dict_setup_similar(void **state) {
	struct dictionary* d = dictionary_new();
	if(!d)
		return -1;
	assert_int_equal(dictionary_insert(d, firstSimilar), 1);
	assert_int_equal(dictionary_insert(d, secondSimilar), 1);
	assert_int_equal(dictionary_insert(d, thirdSimilar), 1);
	assert_int_equal(dictionary_insert(d, forthSimilar), 1);
	assert_int_equal(dictionary_insert(d, fifthSimilar), 1);
	*state = d;
	return 0;
}

/// Niszczy słownik w testach setup_teardown.
static int dict_teardown(void **state) {
	struct dictionary* d = *state;
	dictionary_done(d);
	return 0;
}

/// Test na znajdowanie słów w słowniku.
static void dict_find_test(void **state) {
	struct dictionary* d = *state;
	assert_non_null(d);

	assert_int_equal(dictionary_find(d, L""), 0);

	assert_int_equal(dictionary_find(d, L"x"), 0);
	assert_int_equal(dictionary_find(d, L"az"), 0);
	assert_int_equal(dictionary_find(d, L"axa"), 0);
	assert_int_equal(dictionary_find(d, L"aya"), 0);
	assert_int_equal(dictionary_find(d, L"bm"), 0);
	assert_int_equal(dictionary_find(d, L"bmw"), 0);
	assert_int_equal(dictionary_find(d, L"cd"), 0);
	assert_int_equal(dictionary_find(d, L"cpr"), 0);
	assert_int_equal(dictionary_find(d, L"cpqt"), 0);

	assert_int_equal(dictionary_find(d, L"c"), 1);
	assert_int_equal(dictionary_find(d, L"C"), 1);

	assert_int_equal(dictionary_find(d, L"ax"), 1);
	assert_int_equal(dictionary_find(d, L"Ax"), 1);
	assert_int_equal(dictionary_find(d, L"AX"), 1);    
	assert_int_equal(dictionary_find(d, L"aX"), 1);

	assert_int_equal(dictionary_find(d, L"Cpq"), 1);
	assert_int_equal(dictionary_find(d, L"cPq"), 1);
	assert_int_equal(dictionary_find(d, L"cpQ"), 1);
	assert_int_equal(dictionary_find(d, L"CPq"), 1);
	assert_int_equal(dictionary_find(d, L"CpQ"), 1);
	assert_int_equal(dictionary_find(d, L"cPQ"), 1);
	assert_int_equal(dictionary_find(d, L"CPQ"), 1);

	assert_int_equal(dictionary_find(d, L"b"), 1);
	assert_int_equal(dictionary_find(d, L"B"), 1);

	const wchar_t* veryLong = 
		L"verylongwordnotintreewithpolishcharsxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxqwertyuiopąęćńźżół";
	
	assert_int_equal(dictionary_find(d, veryLong), 0);
	assert_int_equal(dictionary_insert(d, veryLong), 1);
	assert_int_equal(dictionary_find(d, veryLong), 1);

}

/// Test na usuwanie słów ze słownika.
static void dict_delete_test(void** state) {
	struct dictionary* d = *state;
	assert_int_equal(dictionary_delete(d, first), 1);
	assert_int_equal(dictionary_find(d, first), 0);
	assert_int_equal(dictionary_insert(d, first), 1);
	assert_int_equal(dictionary_find(d, first), 1);

	assert_int_equal(dictionary_delete(d, second), 1);
	assert_int_equal(dictionary_find(d, second), 0);
	assert_int_equal(dictionary_insert(d, second), 1);
	assert_int_equal(dictionary_find(d, second), 1);

	assert_int_equal(dictionary_delete(d, third), 1);
	assert_int_equal(dictionary_find(d, third), 0);
	assert_int_equal(dictionary_insert(d, third), 1);
	assert_int_equal(dictionary_find(d, third), 1);

	assert_int_equal(dictionary_delete(d, forth), 1);
	assert_int_equal(dictionary_find(d, forth), 0);
	assert_int_equal(dictionary_insert(d, forth), 1);
	assert_int_equal(dictionary_find(d, forth), 1);

	assert_int_equal(dictionary_delete(d, fifth), 1);
	assert_int_equal(dictionary_find(d, fifth), 0);
	assert_int_equal(dictionary_insert(d, fifth), 1);
	assert_int_equal(dictionary_find(d, fifth), 1);

	assert_int_equal(dictionary_delete(d, first), 1);
	assert_int_equal(dictionary_find(d, first), 0);
	assert_int_equal(dictionary_delete(d, first), 0);
	assert_int_equal(dictionary_find(d, first), 0);
	assert_int_equal(dictionary_insert(d, first), 1);
	assert_int_equal(dictionary_find(d, first), 1);

	assert_int_equal(dictionary_delete(d, L""), 0);
	assert_int_equal(dictionary_delete(d, L"x"), 0);
	assert_int_equal(dictionary_delete(d, L"az"), 0);
	assert_int_equal(dictionary_delete(d, L"axa"), 0);
	assert_int_equal(dictionary_delete(d, L"aya"), 0);
	assert_int_equal(dictionary_delete(d, L"bm"), 0);
	assert_int_equal(dictionary_delete(d, L"bmw"), 0);
	assert_int_equal(dictionary_delete(d, L"cd"), 0);
	assert_int_equal(dictionary_delete(d, L"cpr"), 0);
	assert_int_equal(dictionary_delete(d, L"cpqt"), 0);
   
}

/// Testuje dodawanie słów do słownika po wszystkich ścieżkach drzewa.
static void dict_insert_test(void** state) {
	struct dictionary* d = *state;
	
	assert_int_equal(dictionary_insert(d, L""), 0);

	assert_int_equal(dictionary_insert(d, L"x"), 1);
	assert_int_equal(dictionary_find(d, L"x"), 1);

	assert_int_equal(dictionary_insert(d, L"az"), 1);
	assert_int_equal(dictionary_find(d, L"az"), 1);

	assert_int_equal(dictionary_insert(d, L"axa"), 1);
	assert_int_equal(dictionary_find(d, L"axa"), 1);

	assert_int_equal(dictionary_insert(d, L"aya"), 1);
	assert_int_equal(dictionary_find(d, L"aya"), 1);

	assert_int_equal(dictionary_insert(d, L"bm"), 1);
	assert_int_equal(dictionary_find(d, L"bm"), 1);

	assert_int_equal(dictionary_insert(d, L"bmw"), 1);
	assert_int_equal(dictionary_find(d, L"bmw"), 1);

	assert_int_equal(dictionary_insert(d, L"cd"), 1);
	assert_int_equal(dictionary_find(d, L"cd"), 1);

	assert_int_equal(dictionary_insert(d, L"cpr"), 1);
	assert_int_equal(dictionary_find(d, L"cpr"), 1);

	assert_int_equal(dictionary_insert(d, L"cpqt"), 1);
	assert_int_equal(dictionary_find(d, L"cpqt"), 1);


	assert_int_equal(dictionary_insert(d, L"c"), 0);
	assert_int_equal(dictionary_insert(d, L"C"), 0);

	assert_int_equal(dictionary_insert(d, L"ax"), 0);
	assert_int_equal(dictionary_insert(d, L"Ax"), 0);
	assert_int_equal(dictionary_insert(d, L"AX"), 0);    
	assert_int_equal(dictionary_insert(d, L"aX"), 0);

	assert_int_equal(dictionary_insert(d, L"Cpq"), 0);
	assert_int_equal(dictionary_insert(d, L"cPq"), 0);
	assert_int_equal(dictionary_insert(d, L"cpQ"), 0);
	assert_int_equal(dictionary_insert(d, L"CPq"), 0);
	assert_int_equal(dictionary_insert(d, L"CpQ"), 0);
	assert_int_equal(dictionary_insert(d, L"cPQ"), 0);
	assert_int_equal(dictionary_insert(d, L"CPQ"), 0);

	assert_int_equal(dictionary_insert(d, L"b"), 0);
	assert_int_equal(dictionary_insert(d, L"B"), 0);

}

/// Testuje listę podpowiedzi do różnych słów
static void dict_hints_test(void **state) {
	struct dictionary* d = *state;
	struct word_list* l = malloc(sizeof(struct word_list));

	dictionary_hints(d, L"aab", l);
	assert_int_equal(word_list_size(l), 3);
	const wchar_t* const* array = word_list_get(l);
	assert_int_equal(wcscmp(array[0],  L"aa"), 0);
	assert_int_equal(wcscmp(array[1],  L"ab"), 0);
	assert_int_equal(wcscmp(array[2],  L"cab"), 0);
	word_list_done(l);

	dictionary_hints(d, L"aa", l);
	assert_int_equal(word_list_size(l), 2);
	array = word_list_get(l);
	assert_int_equal(wcscmp(array[0],  L"aa"), 0);
	assert_int_equal(wcscmp(array[1],  L"ab"), 0);
	word_list_done(l);

	dictionary_hints(d, L"ab", l);
	assert_int_equal(word_list_size(l), 4);
	array = word_list_get(l);
	assert_int_equal(wcscmp(array[0],  L"aa"), 0);
	assert_int_equal(wcscmp(array[1],  L"ab"), 0);
	assert_int_equal(wcscmp(array[2],  L"b"), 0);
	assert_int_equal(wcscmp(array[3],  L"cab"), 0);
	word_list_done(l);

	dictionary_hints(d, L"ca", l);
	assert_int_equal(word_list_size(l), 3);
	array = word_list_get(l);
	assert_int_equal(wcscmp(array[0],  L"aa"), 0);
	assert_int_equal(wcscmp(array[1],  L"c"), 0);
	assert_int_equal(wcscmp(array[2],  L"cab"), 0);
	word_list_done(l);

	dictionary_hints(d, L"a", l);
	assert_int_equal(word_list_size(l), 4);
	array = word_list_get(l);
	assert_int_equal(wcscmp(array[0],  L"aa"), 0);
	assert_int_equal(wcscmp(array[1],  L"ab"), 0);
	assert_int_equal(wcscmp(array[2],  L"b"), 0);
	assert_int_equal(wcscmp(array[3],  L"c"), 0);
	word_list_done(l);
	free(l);
}

/** Makro dla czytelności. */
#define run_dict_test(x) cmocka_unit_test_setup_teardown(x, dict_setup, \
	dict_teardown)

/// Funkcja main testów.
int main(void) {
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(dict_new_done_test),
		cmocka_unit_test(dict_setup_done_test),

		run_dict_test(dict_delete_test),
		run_dict_test(dict_insert_test),
		run_dict_test(dict_find_test),
		cmocka_unit_test_setup_teardown(dict_hints_test, dict_setup_similar,
			dict_teardown)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
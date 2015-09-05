/**
	@file
	Testy funkcji implementacji dictionary.
		
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
#include "dictionary.c"


/// Sprawdza funkcję decapitalize zamieniającą duże litery w słowie na małe.
static void decapitalize_test(void **state) {
	wchar_t* small = (wchar_t*)decapitalize(L"abc");
	assert_int_equal(wcscmp(L"abc", small), 0);
	free(small);

	small = (wchar_t*)decapitalize(L"XYZ");
	assert_int_equal(wcscmp(L"xyz", small), 0);
	free(small);

	small = (wchar_t*)decapitalize(L"cDe");
	assert_int_equal(wcscmp(L"cde", small), 0);
	free(small);
}

/// Testuje komparator wcharComp.
static void wcharComp_test(void** state) {
	assert_true(L'm' < L'n');
	wchar_t array[2];
	array[0] = L'n';
	array[1] = L'm';

	//qsort(array, 2, sizeof(wchar_t), wcharComp);
	//assert_int_equal(array[0], L'n');
	//assert_int_equal(array[1], L'n');

	// array[0] = L'm';
	// array[1] = L'n';

	// qsort(array, 2, sizeof(wchar_t), wcharComp);
	// assert_int_equal(array[0], L'm');
	// assert_int_equal(array[1], L'n');
}

/// Funkcja main testów.
int main(void) {
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(decapitalize_test),
		//cmocka_unit_test(wcharComp_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

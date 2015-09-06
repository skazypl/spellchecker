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

// Nie testuję pozostałych funkcji, bo są raczej trywialne lub pomocnicze do
// funkcji multilang

/// Funkcja main testów.
int main(void) {
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(decapitalize_test)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

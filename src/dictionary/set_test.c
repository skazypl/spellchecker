/**
	@file
	Testy funkcji interfejsu set.h
	
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
#include "set.h"

/// Ustawia set do testów setup_teardown.
static int set_setup(void** state) {
	struct InsertSet* s = (struct InsertSet*)malloc(sizeof(struct InsertSet));
	if(!s)
		return -1;
	set_init(s);
	*state = s;
	return 0;
}

/// Niszczy set w testach setup_teardown.
static int set_teardown(void** state) {
	struct InsertSet* s = *state;
	set_done(s);
	free(s);
	return 0;	
}

/// Testuje dodawanie do zbioru.
static void set_add_test(void** state) {
	struct InsertSet* s = *state;
	assert_int_equal(s->size, 0);
	assert_int_equal(s->buffer_size, 1);

	set_add(s, L'x');
	assert_int_equal(s->size, 1);
	assert_int_equal(s->buffer_size, 1);

	set_add(s, L'y');
	assert_int_equal(s->size, 2);
	assert_int_equal(s->buffer_size, 2);

	set_add(s, L'z');
	assert_int_equal(s->size, 3);
	assert_int_equal(s->buffer_size, 4);

	set_add(s, L'y');
	assert_int_equal(s->size, 3);
	assert_int_equal(s->buffer_size, 4);
} 

/// Testuje inicjalizację zbioru.
static void set_init_test(void** state) {
	struct InsertSet* s = (struct InsertSet*)malloc(sizeof(struct InsertSet));
	assert_true(s != NULL);
	set_init(s);
	assert_int_equal(s->size, 0);
	assert_int_equal(s->buffer_size, 1);
	set_done(s);
	free(s);
}

/// Funkcja main testów.
int main(void) {
    setlocale(LC_ALL, "pl_PL.UTF-8");
    const struct CMUnitTest tests[] = {
    	cmocka_unit_test(set_init_test),
		cmocka_unit_test_setup_teardown(set_add_test, set_setup, set_teardown)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
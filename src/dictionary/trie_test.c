#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <cmocka.h>
#include <stdio.h>
#include "trie.h"

const wchar_t* first  =  L"masą";
const wchar_t* second =  L"ma";
const wchar_t* third  =  L"mat";
const wchar_t* forth  =  L"noga";
const wchar_t* fifth  =  L"noś";
const wchar_t* sixth  =  L"nota";

static void trie_init_test(void **state) {
    struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));

    Tree_init(t);
    assert_non_null(t);    
    assert_int_equal(Tree_size(t), 1);
    assert_int_equal(add(t, first), 1);
    assert_int_equal(Tree_size(t), 6);
    assert_int_equal(add(t, second), 1);
    assert_int_equal(Tree_size(t), 7);
    assert_int_equal(add(t, third), 1);
    assert_int_equal(Tree_size(t), 9);
    assert_int_equal(add(t, forth), 1);
    assert_int_equal(Tree_size(t), 14);
    assert_int_equal(add(t, fifth) , 1);
    assert_int_equal(Tree_size(t), 16);
    assert_int_equal(add(t, sixth), 1);
    assert_int_equal(Tree_size(t), 19);
    Tree_destroy(t);
    free(t);
}

static int trie_setup(void **state) {
    struct Tree* t = (struct Tree*)malloc(sizeof(struct Tree));
    if (!t)
        return -1;
    Tree_init(t);
    assert_non_null(t);
    assert_int_equal(Tree_size(t), 1);
    add(t, first);
    assert_int_equal(Tree_size(t), 6);
    add(t, second);
    assert_int_equal(Tree_size(t), 7);
    add(t, third);
    assert_int_equal(Tree_size(t), 9);
    add(t, forth);
    assert_int_equal(Tree_size(t), 14);
    add(t, fifth);  
    assert_int_equal(Tree_size(t), 16);
    add(t, sixth);
    assert_int_equal(Tree_size(t), 19);
    *state = t;
    return 0;
}

static int trie_teardown(void **state) {
    struct Tree* t = *state;
    Tree_destroy(t);
    free(t);
    return 0;
}


static void trie_add_test(void** state) {
    struct Tree* t = *state;
    assert_int_equal(Tree_size(t), 19);
    assert_int_equal(add(t, L""), 0);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(add(t, first ), 0);
    assert_int_equal(add(t, second), 0);
    assert_int_equal(add(t, third ), 0);
    assert_int_equal(add(t, forth ), 0);
    assert_int_equal(add(t, fifth ), 0);
    assert_int_equal(add(t, sixth ), 0);

}

static void trie_find_test(void** state) {
    struct Tree* t = *state;
    assert_int_equal(find(t, first ), 1);
    assert_int_equal(find(t, second), 1);
    assert_int_equal(find(t, third ), 1);
    assert_int_equal(find(t, forth ), 1);
    assert_int_equal(find(t, fifth ), 1);
    assert_int_equal(find(t, sixth ), 1);

    assert_int_equal(find(t, L"" ), 0);
    assert_int_equal(find(t, L"mas" ), 0);
    assert_int_equal(find(t, L"masy" ), 0);
    assert_int_equal(find(t, L"no" ), 0);
    assert_int_equal(find(t, L"nog" ), 0);
    assert_int_equal(find(t, L"notaa" ), 0);
    assert_int_equal(find(t, L"notatka" ), 0);
    assert_int_equal(find(t, L"abcd" ), 0);
    assert_int_equal(find(t, L"x" ), 0);
}


#define run_trie_test(x) cmocka_unit_test_setup_teardown(x, \
    trie_setup, trie_teardown)

int main(void) {
    setlocale(LC_ALL, "pl_PL.UTF-8");
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(trie_init_test),
        //cmocka_unit_test(trie_destroy_test),
        run_trie_test(trie_add_test),
        run_trie_test(trie_find_test)

    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
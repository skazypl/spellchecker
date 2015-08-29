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

    assert_int_equal(add(t, first), 0);
    assert_int_equal(add(t, second), 0);
    assert_int_equal(add(t, third), 0);
    assert_int_equal(add(t, forth), 0);
    assert_int_equal(add(t, fifth), 0);
    assert_int_equal(add(t, sixth), 0);

    assert_int_equal(add(t, L""), 0);
    assert_int_equal(add(t, L"x"), 1);
    assert_int_equal(add(t, L"mi"), 1);
    assert_int_equal(add(t, L"mas"), 1);
    assert_int_equal(add(t, L"max"), 1);
    assert_int_equal(add(t, L"masy"), 1);
    assert_int_equal(add(t, L"mata"), 1);
    assert_int_equal(add(t, L"masąc"), 1);
    assert_int_equal(add(t, L"nie"), 1);
    assert_int_equal(add(t, L"no"), 1);
    assert_int_equal(add(t, L"na"), 1);
    assert_int_equal(add(t, L"nog"), 1);
    assert_int_equal(add(t, L"not"), 1); 
    assert_int_equal(add(t, L"nora"), 1);
    assert_int_equal(add(t, L"notaa"), 1);
    assert_int_equal(add(t, L"nośnik"), 1);
    assert_int_equal(add(t, L"nogami"), 1);
    assert_int_equal(add(t, L"notatka"), 1);

    const wchar_t* veryLong = 
        L"verylongwordnotintreewithpolishcharsxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
        xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxqwertyuiopąęćńźżół";
  
    assert_int_equal(add(t, veryLong), 1);
}

static void trie_find_test(void** state) {
    struct Tree* t = *state;
    assert_int_equal(find(t, first), 1);
    assert_int_equal(find(t, second), 1);
    assert_int_equal(find(t, third), 1);
    assert_int_equal(find(t, forth), 1);
    assert_int_equal(find(t, fifth), 1);
    assert_int_equal(find(t, sixth), 1);

    assert_int_equal(find(t, L""), 0);
    assert_int_equal(find(t, L"x"), 0);
    assert_int_equal(find(t, L"mi"), 0);
    assert_int_equal(find(t, L"mas"), 0);
    assert_int_equal(find(t, L"max"), 0);
    assert_int_equal(find(t, L"masy"), 0);
    assert_int_equal(find(t, L"mata"), 0);
    assert_int_equal(find(t, L"masąc"), 0);
    assert_int_equal(find(t, L"nie"), 0);
    assert_int_equal(find(t, L"no"), 0);
    assert_int_equal(find(t, L"na"), 0);
    assert_int_equal(find(t, L"nog"), 0);
    assert_int_equal(find(t, L"not"), 0);    
    assert_int_equal(find(t, L"nora"), 0);
    assert_int_equal(find(t, L"notaa"), 0);
    assert_int_equal(find(t, L"nośnik"), 0);
    assert_int_equal(find(t, L"nogami"), 0);
    assert_int_equal(find(t, L"notatka"), 0);

    const wchar_t* veryLong = 
        L"verylongwordnotintreewithpolishcharsxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
        xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxqwertyuiopąęćńźżół";

    assert_int_equal(find(t, veryLong), 0);
    assert_int_equal(add(t, veryLong), 1);
    assert_int_equal(find(t, veryLong), 1);

}

static void trie_delete_test(void** state) {
    struct Tree* t = *state;
    assert_int_equal(delete(t, first), 1);
    assert_int_equal(find(t, first), 0);
    assert_int_equal(add(t, first), 1);
    assert_int_equal(find(t, first), 1);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(delete(t, second), 1);
    assert_int_equal(find(t, second), 0);
    assert_int_equal(add(t, second), 1);
    assert_int_equal(find(t, second), 1);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(delete(t, third), 1);
    assert_int_equal(find(t, third), 0);
    assert_int_equal(add(t, third), 1);
    assert_int_equal(find(t, third), 1);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(delete(t, forth), 1);
    assert_int_equal(find(t, forth), 0);
    assert_int_equal(add(t, forth), 1);
    assert_int_equal(find(t, forth), 1);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(delete(t, fifth), 1);
    assert_int_equal(find(t, fifth), 0);
    assert_int_equal(add(t, fifth), 1);
    assert_int_equal(find(t, fifth), 1);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(delete(t, sixth), 1);
    assert_int_equal(find(t, sixth), 0);
    assert_int_equal(add(t, sixth), 1);
    assert_int_equal(find(t, sixth), 1);
    assert_int_equal(Tree_size(t), 19);

    assert_int_equal(delete(t, first), 1);
    assert_int_equal(find(t, first), 0);
    assert_int_equal(delete(t, first), 0);
    assert_int_equal(find(t, first), 0);
    assert_int_equal(add(t, first), 1);
    assert_int_equal(find(t, first), 1);
    assert_int_equal(Tree_size(t), 19);


    assert_int_equal(delete(t, L""), 0);
    assert_int_equal(delete(t, L"x"), 0);
    assert_int_equal(delete(t, L"mi"), 0);
    assert_int_equal(delete(t, L"mas"), 0);
    assert_int_equal(delete(t, L"max"), 0);
    assert_int_equal(delete(t, L"masy"), 0);
    assert_int_equal(delete(t, L"mata"), 0);
    assert_int_equal(delete(t, L"masąc"), 0);
    assert_int_equal(delete(t, L"nie"), 0);
    assert_int_equal(delete(t, L"no"), 0);
    assert_int_equal(delete(t, L"na"), 0);
    assert_int_equal(delete(t, L"nog"), 0);
    assert_int_equal(delete(t, L"not"), 0);    
    assert_int_equal(delete(t, L"nora"), 0);
    assert_int_equal(delete(t, L"notaa"), 0);
    assert_int_equal(delete(t, L"nośnik"), 0);
    assert_int_equal(delete(t, L"nogami"), 0);
    assert_int_equal(delete(t, L"notatka"), 0);
}

/* na razie nie testujemy usedInTree() - w zasadzie nie ma czego testować.
*/


#define run_trie_test(x) cmocka_unit_test_setup_teardown(x, trie_setup, \
    trie_teardown)

int main(void) {
    setlocale(LC_ALL, "pl_PL.UTF-8");
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(trie_init_test),
        //cmocka_unit_test(trie_destroy_test),
        run_trie_test(trie_add_test),
        run_trie_test(trie_find_test),
        run_trie_test(trie_delete_test)

    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
/**
	@file
	Testy funkcji pomocniczych trie
	
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
#include "trie.c"

const wchar_t* emptyWord = L"";///< Wstawiany string.
const wchar_t* oneLetter = L"x";///< Wstawiany string.
const wchar_t* kind      = L"kind";///< Wstawiany string.
const wchar_t* kinder    = L"kinder";///< Wstawiany string.
const wchar_t* kiss      = L"kiss";///< Wstawiany string.

/// Testuje tworzenie węzła funkcją createNode i jego niszczenie.
static void createNode_test(void** state) {
	struct Node* n = createNode(0, '\0', NULL);
	assert_non_null(n);
	Node_destroy(n);

	n = createNode(1, '\0', NULL);
	assert_non_null(n);
	struct Node* m = createNode(0, '\0', n);
	n->children[0] = m;
	assert_non_null(m);
	assert_int_equal(m->parent, n);
	Node_destroy(n);

	n = createNode(-1, 0, NULL);
	assert_true(n == NULL);
}

/// Testuje samo niszczenie węzła.
static void Node_destroy_test(void** state) {
	struct Node* n = createNode(1, L'a', NULL);
	struct Node* m = createNode(1, L'b', NULL);
	struct Node* o = createNode(0, L'c', NULL);
	n->children[0] = m;
	m->children[0] = o;
	Node_destroy(n);
}

/// Testuje komparator nodeComp.
static void nodeComp_test(void** state) {
	assert_true(L'a' < L'z');
	struct Node* n = createNode(0, L'a', NULL);
	struct Node* m = createNode(0, L'z', NULL);
	struct Node* array[2];
	array[0] = n;
	array[1] = m;

	qsort(array, 2, sizeof(struct Node*), nodeComp);
	assert_int_equal(array[0], n);
	assert_int_equal(array[1], m);

	array[0] = m;
	array[1] = n;

	qsort(array, 2, sizeof(struct Node*), nodeComp);
	assert_int_equal(array[0], n);
	assert_int_equal(array[1], m);

	free(n);
	free(m);
}

/// Testuje wyszukiwanie binarne w posortowanej tablicy.
static void binSearch_test(void** state) {
	struct Node* array[2];
	array[0] = createNode(0, L'c', NULL);
	array[1] = createNode(0, L'd', NULL);

	assert_int_equal(binSearch(array, 2, 'c'), 0);
	assert_int_equal(binSearch(array, 2, 'd'), 1);
	assert_int_equal(binSearch(array, 2, 'z'), -1);
	assert_int_equal(binSearch(array, 2, 'a'), -1);

	free(array[0]);
	free(array[1]);

	struct Node* nextArray[3];
	nextArray[0] = createNode(0, L'b', NULL);
	nextArray[1] = createNode(0, L'c', NULL);
	nextArray[2] = createNode(0, L'd', NULL);  

	assert_int_equal(binSearch(nextArray, 3, 'b'), 0);
	assert_int_equal(binSearch(nextArray, 3, 'c'), 1);
	assert_int_equal(binSearch(nextArray, 3, 'd'), 2);
	assert_int_equal(binSearch(nextArray, 3, 'z'), -1);
	assert_int_equal(binSearch(nextArray, 3, 'a'), -1);

	free(nextArray[0]);
	free(nextArray[1]);
	free(nextArray[2]);
}

/// Testuje dodawanie węzła metodą addNode.
static void addNode_test(void** state) {
	struct Node* m = createNode(1, 0, NULL);
	struct Node* n = createNode(0, L'k', m);
	m->children[0] = n;

	assert_int_equal(addNode(m, kind), 1);
	struct Node* temp = n->children[0];

	assert_int_equal(temp->key, 'i');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'n');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'd');
	temp = temp->children[0];

	assert_int_equal(temp->key, '\0');
	assert_int_equal(temp->childCount, 0);

	Node_destroy(m);
	m = createNode(1, 0, NULL);
	n = createNode(1, 'l', NULL);
	m->children[0] = n;
	assert_int_equal(addNode(m, kind), 1);
	temp = m->children[0];

	assert_int_equal(temp->key, 'k');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'i');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'n');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'd');
	temp = temp->children[0];

	assert_int_equal(temp->key, '\0');
	assert_int_equal(temp->childCount, 0);

	Node_destroy(m);
	m = createNode(1, 0, NULL);
	n = createNode(1, 'j', NULL);
	m->children[0] = n;
	assert_int_equal(addNode(m, kind), 1);
	assert_int_equal(m->childCount, 2);
	temp = m->children[1];

	assert_int_equal(temp->key, L'k');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'i');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'n');
	temp = temp->children[0];

	assert_int_equal(temp->key, 'd');
	temp = temp->children[0];

	assert_int_equal(temp->key, '\0');
	assert_int_equal(temp->childCount, 0);


	Node_destroy(m);
	m = createNode(1, 0, NULL);
	n = createNode(1, 'j', NULL);
	m->children[0] = n;
	assert_int_equal(addNode(m, emptyWord), 1);
	temp = m->children[0];
	assert_int_equal(temp->key, '\0');
	
	temp = m->children[1];
	assert_int_equal(temp->key, 'j');


	Node_destroy(m);
}

/// Ustawia węzły do testów setup_teardown.
static int node_setup(void** state) {
	struct Node* n = malloc(sizeof(struct Node));
	Node_init(n);
	addNode(n, kind);
	addNode(n, kinder);
	addNode(n, kiss);
	*state = n;
	return 0;
}

/// Niszczy strukturę węzłów w testach setup_teardown.
static int node_teardown(void** state) {
	struct Node* n = *state;
	Node_destroy(n);
	return 0;
}

/// Testuje wyszukiwanie węzła za pomocą funkcji findNode.
static void findNode_test(void** state) {
	struct Node* m = malloc(sizeof(struct Node));
	Node_init(m);
	addNode(m, kind);
	assert_int_equal(findNode(m, kind), 1);
	assert_int_equal(findNode(m, kinder), 0);
	assert_int_equal(findNode(m, kiss), 0);

	Node_destroy(m);
	m = malloc(sizeof(struct Node));
	Node_init(m);
	addNode(m, kinder);
	assert_int_equal(findNode(m, kind), 0);
	assert_int_equal(findNode(m, kinder), 1);
	assert_int_equal(findNode(m, kiss), 0);
	addNode(m, kiss);
	assert_int_equal(findNode(m, kiss), 1);
	assert_int_equal(findNode(m, kinder), 1);
	addNode(m, kind);
	assert_int_equal(findNode(m, kind), 1);
	assert_int_equal(findNode(m, kinder), 1);
	assert_int_equal(findNode(m, kiss), 1);

	Node_destroy(m);
}

/// Testuje wyszukiwanie liścia kończącego słowo za pomocą findLeaf.
static void findLeaf_test(void** state) {
	struct Node* n = *state;
	struct Node* leaf = findLeaf(n, kind);
	struct Node* temp = n;
	temp = temp->children[0]; // k
	temp = temp->children[0]; // i
	temp = temp->children[0]; // n
	temp = temp->children[0]; // d
	temp = temp->children[0]; // \0
	assert_int_equal(leaf, temp);

	leaf = findLeaf(n, kinder);
	temp = n;
	temp = temp->children[0]; // k
	temp = temp->children[0]; // i
	temp = temp->children[0]; // n
	temp = temp->children[0]; // d
	temp = temp->children[1]; // e
	temp = temp->children[0]; // r
	temp = temp->children[0]; // \0
	assert_int_equal(leaf, temp);

	leaf = findLeaf(n, kiss);
	temp = n;
	temp = temp->children[0]; // k
	temp = temp->children[0]; // i
	temp = temp->children[1]; // s
	temp = temp->children[0]; // s
	temp = temp->children[0]; // \0
	assert_int_equal(leaf, temp);
}

/// Testuje sprawdzanie rozmiaru poddrzewa.
static void NodeSize_test(void** state) {
	struct Node* n = *state;
	assert_int_equal(NodeSize(n), 12);

	struct Node* m = createNode(0, 0, NULL);
	assert_int_equal(NodeSize(m), 1);
	Node_destroy(m);

	assert_int_equal(NodeSize(NULL), 0);
}

/** Makro dla czytelności. */
#define run_node_test(x) cmocka_unit_test_setup_teardown(x, node_setup, \
	node_teardown)

/// Funkcja main testów.
int main(void) {
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(createNode_test),
		cmocka_unit_test(Node_destroy_test),
		cmocka_unit_test(nodeComp_test),
		cmocka_unit_test(binSearch_test),
		cmocka_unit_test(addNode_test),
		cmocka_unit_test(findNode_test),
		run_node_test(findLeaf_test),
		run_node_test(NodeSize_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
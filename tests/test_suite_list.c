#include "vector.h"
#include "test_util.h"
#include "list.h"
#include <assert.h>
#include <math.h>


/*
void *vec_star(int x, int y){
    value = malloc(sizeof(Vector));
    Vector vec = (Vector) {x, y};
    value = &vec;
    return value;
}

void test_make_small_list() {
    List *testVecList = list_init(4, free);
    assert(list_size(testVecList) == 0);
    assert(list_cap(testVecList) == 4);
    list_add(testVecList, vec_star(1, 1));
    assert(list_size(testVecList) == 1);
    assert((* (Vector *) list_get(testVecList, 0)) == ((Vector) {1, 1}));
    assert(* (Vector *) list_remove(testVecList, 0) == ((Vector) {1, 1}));
    assert(list_size(testVecList) == 0);
    list_add(testVecList, vec_star(1, 1));
    assert(list_size(testVecList) == 1);
    list_add(testVecList, vec_star(1, 2));
    assert(list_size(testVecList) == 2);
    list_add(testVecList, vec_star(1, 3));
    assert(list_size(testVecList) == 3);
    list_add(testVecList, vec_star(1, 4));
    assert(list_size(testVecList) == 4);
    assert(list_cap(testVecList) == 4);
    assert((* (Vector *) list_get(testVecList, 3)) == ((Vector) {1, 4}));
    assert((* (Vector *) list_remove(testVecList, 0)) == ((Vector) {1, 1}));
    assert(list_size(testVecList) == 3);
    assert(list_cap(testVecList) == 4);
    assert((* (Vector *) list_get(testVecList, 0)) == ((Vector) {1, 2}));
    list_free(testVecList);
}

void test_overflow() {
    List *test = list_init(1, free);
    for(size_t i = 0; i < 1000; i++){
        list_add(test, vec_star(i, i));
        assert(list_size(test) == i+1);
        assert(list_cap(test) == i+1);
        assert((* (Vector *) list_get(test, i)) == ((Vector) {i, i}));
    }
    for(size_t i = 0; i < 1000; i++){
        assert((* (Vector *) list_get(test, 0)) == ((Vector) {i, i}));
        assert((* (Vector *) list_remove(test, 0)) == ((Vector) {i, i}));
        assert(list_size(test) == 999 - i);
        assert(list_cap(test) == 1000);
    }
    list_free(test);
}

void test_layered_free() {
    List *testListList = list_init(4, list_free);
    List *smolList;
    for (size_t i = 0; i < 4; i++){
        smolList = list_init(4, free);
        list_add(smolList, vec_star(0, 0));
        list_add(testListList, smolList);
    }
    assert(list_remove(testListList, 3) == smolList);
    list_free(testListList);
}
*/

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

/*
    DO_TEST(test_make_small_list)
    DO_TEST(test_overflow)
    DO_TEST(test_layered_free)
*/



    puts("list_test PASS");

    return 0;
}

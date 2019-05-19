#include "vector.h"
#include "test_util.h"
#include "list.h"
#include "forces.h"
#include "collision.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

void test_collision() {
    Vector v[] = {{1, 1}, {3, 1}, {3, 3}, {1, 3}};
    Vector v2[] = {{2, 2}, {4, 2}, {4, 4}, {2, 4}};
    const size_t VERTICES = sizeof(v) / sizeof(*v);
    List *shape = list_init(1, free);
    List *shape2 = list_init(1, free);
    for (size_t i = 0; i < VERTICES; i++) {
        Vector *list_v = malloc(sizeof(*list_v));
        *list_v = v[i];
        list_add(shape, list_v);

        Vector *list_v2 = malloc(sizeof(*list_v2));
        *list_v2 = v2[i];
        list_add(shape2, list_v2);
    }
    assert(find_collision(shape, shape2));
    assert(find_collision(shape2, shape));
    list_free(shape);
    list_free(shape2);
    return;
}

void test_no_collision() {
    Vector v[] = {{1, 1}, {2, 1}, {2, 2}, {1, 2}};
    Vector v2[] = {{5, 5}, {6, 5}, {6, 6}, {5, 6}};
    const size_t VERTICES = sizeof(v) / sizeof(*v);
    List *shape = list_init(1, free);
    List *shape2 = list_init(1, free);
    for (size_t i = 0; i < VERTICES; i++) {
        Vector *list_v = malloc(sizeof(*list_v));
        *list_v = v[i];
        list_add(shape, list_v);

        Vector *list_v2 = malloc(sizeof(*list_v2));
        *list_v2 = v2[i];
        list_add(shape2, list_v2);
    }
    assert(!find_collision(shape, shape2));
    assert(!find_collision(shape2, shape));
    list_free(shape);
    list_free(shape2);
    return;
}

void test_border_collision() {
    //Assumed 'yes' for border collision
    Vector v[] = {{1, 1}, {2, 1}, {2, 2}, {1, 2}};
    Vector v2[] = {{2, 1}, {3, 1}, {3, 2}, {2, 2}};
    const size_t VERTICES = sizeof(v) / sizeof(*v);
    List *shape = list_init(1, free);
    List *shape2 = list_init(1, free);
    for (size_t i = 0; i < VERTICES; i++) {
        Vector *list_v = malloc(sizeof(*list_v));
        *list_v = v[i];
        list_add(shape, list_v);

        Vector *list_v2 = malloc(sizeof(*list_v2));
        *list_v2 = v2[i];
        list_add(shape2, list_v2);
    }
    assert(find_collision(shape, shape2));
    assert(find_collision(shape2, shape));
    list_free(shape);
    list_free(shape2);
    return;
}

void test_corner_collision() {
    //Assumed 'yes' for corner collision
    Vector v[] = {{1, 1}, {2, 1}, {2, 2}, {1, 2}};
    Vector v2[] = {{2, 2}, {3, 2}, {3, 3}, {2, 3}};
    const size_t VERTICES = sizeof(v) / sizeof(*v);
    List *shape = list_init(1, free);
    List *shape2 = list_init(1, free);
    for (size_t i = 0; i < VERTICES; i++) {
        Vector *list_v = malloc(sizeof(*list_v));
        *list_v = v[i];
        list_add(shape, list_v);

        Vector *list_v2 = malloc(sizeof(*list_v2));
        *list_v2 = v2[i];
        list_add(shape2, list_v2);
    }
    assert(find_collision(shape, shape2));
    assert(find_collision(shape2, shape));
    list_free(shape);
    list_free(shape2);
    return;
}

void test_inside_collision() {
    //Assumed 'yes' for inside collision
    Vector v[] = {{1, 1}, {6, 1}, {6, 6}, {1, 6}};
    Vector v2[] = {{2, 2}, {3, 2}, {3, 3}, {2, 3}};
    const size_t VERTICES = sizeof(v) / sizeof(*v);
    List *shape = list_init(1, free);
    List *shape2 = list_init(1, free);
    for (size_t i = 0; i < VERTICES; i++) {
        Vector *list_v = malloc(sizeof(*list_v));
        *list_v = v[i];
        list_add(shape, list_v);

        Vector *list_v2 = malloc(sizeof(*list_v2));
        *list_v2 = v2[i];
        list_add(shape2, list_v2);
    }
    assert(find_collision(shape, shape2));
    assert(find_collision(shape2, shape));
    list_free(shape);
    list_free(shape2);
    return;
}

void test_same_collision() {
    //Assumed 'yes' for same object collision
    Vector v[] = {{1, 1}, {6, 1}, {6, 6}, {1, 6}};
    const size_t VERTICES = sizeof(v) / sizeof(*v);
    List *shape = list_init(1, free);
    for (size_t i = 0; i < VERTICES; i++) {
        Vector *list_v = malloc(sizeof(*list_v));
        *list_v = v[i];
        list_add(shape, list_v);
    }
    assert(find_collision(shape, shape));
    list_free(shape);
    return;
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_collision)
    DO_TEST(test_no_collision)
    DO_TEST(test_border_collision)
    DO_TEST(test_corner_collision)
    DO_TEST(test_inside_collision)
    DO_TEST(test_same_collision)

    puts("collision_test PASS");

    return 0;
}

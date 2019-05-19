#include "forces.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "scene.h"
#include "sdl_wrapper.h"
#include "polygon.h"
#include "body.h"
#include "color.h"
#include "scene.h"
#include "list.h"
#include <vector.h>
#include <string.h>
#include <stdio.h>
#include <time.h>


List *make_shape() {
    List *shape = list_init(4, free);
    Vector *v = malloc(sizeof(*v));
    *v = (Vector) {-1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (Vector) {+1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (Vector) {+1, +1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (Vector) {-1, +1};
    list_add(shape, v);
    return shape;
}

/*
/ Tests that an oscillating spring won't exhibit unexpected amplification with
/ damping
/ Damping: ~ 0.5
/ Spring Constant: ~ 10
/ Mass: 2.0
/ Place high up. Ensure amplitude never exceeds starting point.
/ End after 10 full periods
*/
void test_spring_amp() {
    const double DT = 1e-6;
    const int STEPS = 1000000;
    const double K = 10.0;
    const double GAMMA = 0.5;
    Scene *scene = scene_init();
    Body *invisBody = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    Body *visBody = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody, (Vector) {0, 20});
    scene_add_body(scene, invisBody);
    scene_add_body(scene, visBody);
    create_spring(scene, K, visBody, invisBody);
    create_drag(scene, GAMMA, visBody);
    for(size_t i = 0; i < STEPS; i++){
        scene_tick(scene, DT);
        assert(fabs(body_get_centroid(visBody).y) <= 20);
    }
    scene_free(scene);
}

/*
/ Tests that the spring constant doesn't affect the amplitude
/ Spring Constant: Vary from 20 to 2000
/ Mass: 2.0
/ Place high up. Ensure amplitude gets within 1% of starting coordinate but
/ does not exceed 1% of starting coordinate (End test after 3 full periods)
*/
void test_spring_multi() {
    const double DT = 1e-6;
    const int STEPS = 1000000;
    double k = 20.0;
    Scene *scene = scene_init();
    Body *invisBody = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    Body *visBody = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody, (Vector) {0, 50});
    scene_add_body(scene, invisBody);
    scene_add_body(scene, visBody);
    create_spring(scene, k, visBody, invisBody);
    for(size_t i = 0; i < STEPS; i++){
        scene_tick(scene, DT);
        assert(fabs(body_get_centroid(visBody).y) <= 50.1);
    }
    scene_free(scene);

    k = 200.0;
    scene = scene_init();
    invisBody = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    visBody = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody, (Vector) {0, 100});
    scene_add_body(scene, invisBody);
    scene_add_body(scene, visBody);
    create_spring(scene, k, visBody, invisBody);
    for(size_t i = 0; i < STEPS; i++){
        scene_tick(scene, DT);
        assert(fabs(body_get_centroid(visBody).y) <= 100.1);
    }
    scene_free(scene);

    k = 2000.0;
    scene = scene_init();
    invisBody = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    visBody = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody, (Vector) {0, 150});
    scene_add_body(scene, invisBody);
    scene_add_body(scene, visBody);
    create_spring(scene, k, visBody, invisBody);
    for(size_t i = 0; i < STEPS; i++){
        scene_tick(scene, DT);
        assert(fabs(body_get_centroid(visBody).y) <= 150.1);
    }
    scene_free(scene);
}

// Test amplitude with multiple balls
void test_spring_multi2() {
    const double DT = 1e-6;
    const int STEPS = 1000000;
    double k = 20.0;
    Scene *scene = scene_init();
    Body *invisBody = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    Body *visBody = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody, (Vector) {0, 50});
    scene_add_body(scene, invisBody);
    scene_add_body(scene, visBody);
    create_spring(scene, k, visBody, invisBody);

    Body *invisBody2 = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    Body *visBody2 = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody2, (Vector) {0, 50});
    scene_add_body(scene, invisBody2);
    scene_add_body(scene, visBody2);
    create_spring(scene, k*2, visBody2, invisBody2);

    Body *invisBody3 = body_init(make_shape(), INFINITY, (RGBColor) {0, 0, 0});
    Body *visBody3 = body_init(make_shape(), 2, (RGBColor) {0, 0, 0});
    body_set_centroid(visBody3, (Vector) {0, 50});
    scene_add_body(scene, invisBody3);
    scene_add_body(scene, visBody3);
    create_spring(scene, k*3, visBody3, invisBody3);

    for(size_t i = 0; i < STEPS; i++){
        scene_tick(scene, DT);
        assert(fabs(body_get_centroid(visBody).y) <= 50.1);
        assert(fabs(body_get_centroid(visBody2).y) <= 50.1);
        assert(fabs(body_get_centroid(visBody3).y) <= 50.1);
    }
    scene_free(scene);
}

//Test extreme velocity in n-body system
void test_extreme_velocity() {
    const double M = 4.0;
    const double G = 1e3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    Scene *scene = scene_init();
    Body *mass1 = body_init(make_shape(), M, (RGBColor) {0, 0, 0});
    Body *mass2 = body_init(make_shape(), M, (RGBColor) {0, 0, 0});
    Body *mass3 = body_init(make_shape(), M, (RGBColor) {0, 0, 0});
    scene_add_body(scene, mass1);
    scene_add_body(scene, mass2);
    scene_add_body(scene, mass3);
    create_newtonian_gravity(scene, G, mass1, mass2);
    create_newtonian_gravity(scene, G, mass2, mass3);
    create_newtonian_gravity(scene, G, mass1, mass3);
    for (int i = 0; i < STEPS; i++) {
      assert(body_get_velocity(mass1).x < 10000 && body_get_velocity(mass1).y < 10000);
      assert(body_get_velocity(mass2).x < 10000 && body_get_velocity(mass2).y < 10000);
      assert(body_get_velocity(mass3).x < 10000 && body_get_velocity(mass3).y < 10000);
      scene_tick(scene, DT);
    }
    scene_free(scene);
}

//Tests
int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_spring_amp)
    DO_TEST(test_spring_multi)
    DO_TEST(test_spring_multi2)
    DO_TEST(test_extreme_velocity)

    puts("student_tests PASS");
    return 0;
}

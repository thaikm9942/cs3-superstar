#include "sdl_wrapper.h"
#include "polygon.h"
#include "body.h"
#include "color.h"
#include "scene.h"
#include "list.h"
#include <vector.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "forces.h"
#include "color.h"

const Vector BOUNDARY = {
  .x = 500.0,
  .y = 250.0
};

const double OUTER_RADIUS_MAX = 30;
const double OUTER_RADIUS_MIN = 10;
const double COLOR_FREQ;
const double GRAVITY = 58;
const int NSIDES = 4;
const double NSTARS = 55;
const int START_VEL_MAX = 5;

Vector *init_vec(Vector v){
  Vector *vector = malloc(sizeof(v));
  vector->x = v.x;
  vector->y = v.y;
  return vector;
}
List *rotate_points(Vector point){
  double angle = 2 * M_PI / NSIDES;
  List *rotated = list_init(NSIDES, free);
  for(size_t i = 0; i < NSIDES; i++) {
    list_add(rotated, init_vec(vec_rotate(point, angle * i)));
  }
  return rotated;
}

/* Generates a random value in range (min, max) . Returns an int.*/
int randomValue(int min, int max){
    if(rand() %2 == 1)
        return rand() % (max - min + 1) + min;
    return -1 * rand() % (max - min + 1) + min;

}

Body *init_star(Vector position, double mass, RGBColor color){
    double big_r = randomValue(OUTER_RADIUS_MIN, OUTER_RADIUS_MAX);
    double small_r = big_r / 2;
    Vector outer_point = vec_add(VEC_ZERO, (Vector){0, big_r});
    Vector inner_point = vec_add(VEC_ZERO, (Vector){small_r * cos(M_PI/2 + M_PI/NSIDES), small_r * sin(M_PI/2 + M_PI/NSIDES)});


    List *outer = rotate_points(outer_point);
    List *inner = rotate_points(inner_point);
    List *star = list_init(2 * NSIDES, free);

    // Combines the list of outer and inner points in counterclockwise direction
    for(size_t i = 0; i < NSIDES; i++){
      list_add(star, init_vec(*(Vector*)list_get(outer, i)));
      list_add(star, init_vec(*(Vector*)list_get(inner, (i % NSIDES))));
    }

    list_free(outer);
    list_free(inner);

    polygon_translate(star, position);
    Body * bod = body_init(star, mass, color);
    body_set_velocity(bod, (Vector){randomValue(0, START_VEL_MAX), randomValue(0, START_VEL_MAX)});
    return bod;
}

Scene *init_scene(void){
  Scene *scene = scene_init();
  for(double i = 0; i < NSTARS; i += 1){
    scene_add_body(scene, init_star((Vector){randomValue(50,BOUNDARY.x - 50), randomValue(50,BOUNDARY.y - 50)},
    randomValue(7,20), get_new_color()));
  }
  return scene;
}


void add_forces(Scene *scene){
  Body *bod;
  Body *otherBod;
  for(size_t i = 0; i < scene_bodies(scene); i++) {
    for(size_t k = 0; k < scene_bodies(scene); k++) {
      if(i != k) {
        bod = scene_get_body(scene, i);
        otherBod = scene_get_body(scene, k);
        create_newtonian_gravity(scene, GRAVITY, bod, otherBod);
      }
    }
  }
}

void compute_new_positions(Scene *scene, double dt){
  scene_tick(scene, dt);
}

int main(int argc, char *argv[]){
  srand(time(0));
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  Scene *scene = init_scene();
  add_forces(scene);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    compute_new_positions(scene, dt);
    sdl_clear();
    for(size_t i = 0; i < scene_bodies(scene); i++){
      Body *body = scene_get_body(scene, i);
      List *polygon = body_get_shape(body);
      sdl_draw_polygon(polygon, body_get_color(body));
    }
    sdl_show();
  }
  scene_free(scene);
  return 0;
}

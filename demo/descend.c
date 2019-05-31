#include "sdl_wrapper.h"
#include "forces.h"
#include "color.h"
#include "collision.h"
#include "polygon.h"
#include "body.h"
#include "color.h"
#include "scene.h"
#include "list.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

const Vector BOUNDARY = {
  .x = 100.0,
  .y = 100.0
};

const int NUM_ROWS = 3;
const Vector SPEED = (Vector){20, 0};
const Vector SPEED_UP = (Vector){0, 20};
const Vector BALL_POS = (Vector){0, 50};
const Vector BALL_VEL = (Vector){0, -50};
const Vector BLOCK_VEL = (Vector){0, -20};
const double BALL_MASS = 2;
const double BALL_RADIUS = 5;
const RGBColor BALL_COLOR = (RGBColor){0.95, 0.0, 0.0};
const Vector BLOCK_DIM = (Vector){10, 2};
const double BLOCK_SPACING = 7;
const double COLOR_FREQ = 0.25;
const RGBColor WHITE = (RGBColor){1.0, 1.0, 1.0};

#define G 6.67E-11 // N m^2 / kg^2
#define M 6E24 // kg
#define g 9.8 // m / s^2
#define R (sqrt(G * M / g)) // m

typedef enum {
    BALL,
    BLOCK,
    GRAVITY
} BodyType;

BodyType get_type(Body *body) {
    return *(BodyType *) body_get_info(body);
}

RGBColor rainbow(double seed){
  seed *= COLOR_FREQ;
  return (RGBColor){(1 + sin(seed))/2.0, (1 + sin(seed + 2))/2.0, (1+sin(seed + 4))/2.0};
}

List *create_block(Vector position, Vector dimension){
  List *block = list_init(4, free);
  list_add(block, vec_init((Vector){dimension.x / 2.0, dimension.y / 2.0}));
  list_add(block, vec_init((Vector){dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, dimension.y / 2.0}));
  polygon_translate(block, position);
  return block;
}

Body *init_block(Vector position, Vector dimension, RGBColor color){
  BodyType *type = malloc(sizeof(*type));
  *type = BLOCK;
  return body_init_with_info(create_block(position, dimension), INFINITY, color, type, free);
}

Body *init_ball(Vector position, double mass, double radius, RGBColor color){
  List *ball = list_init(75, free);
  BodyType *type = malloc(sizeof(*type));
  *type = BALL;
  for(double angle = 0.0; angle < 2 * M_PI; angle += 0.05){
    list_add(ball, vec_init(vec_multiply(radius, (Vector){cos(angle), sin(angle)})));
  }
  polygon_translate(ball, position);
  return body_init_with_info(ball, mass, color, type, free);
}

/** Constructs a rectangle with the given dimensions centered at (0, 0) */
List *rect_init(double width, double height) {
    Vector half_width  = {.x = width / 2, .y = 0.0},
           half_height = {.x = 0.0, .y = height / 2};
    List *rect = list_init(4, free);
    Vector *v = malloc(sizeof(*v));
    *v = vec_add(half_width, half_height);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_height, half_width);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_negate(*(Vector *) list_get(rect, 0));
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_width, half_height);
    list_add(rect, v);
    return rect;
}

Body *get_gravity_body() {
    // Will be offscreen, so shape is irrelevant
    List *gravity_ball = rect_init(1, 1);
    BodyType *type = malloc(sizeof(*type));
    *type = GRAVITY;
    Body *body = body_init_with_info(gravity_ball, M, WHITE, type, free);

    // Move a distnace R below the scene
    Vector gravity_center = {.x = BOUNDARY.x / 2, .y = -R};
    body_set_centroid(body, gravity_center);

    return body;
}

Scene *init_scene(Scene *scene){
  Body *ball = init_ball(BALL_POS, BALL_MASS, BALL_RADIUS, BALL_COLOR);
  body_set_velocity(ball, BALL_VEL);
  scene_add_body(scene, ball);
  create_newtonian_gravity(scene, G, scene_get_body(scene, 0), ball);
  Body *block = init_block((Vector){0, 20}, BLOCK_DIM, rainbow(100));
  body_set_velocity(block, BLOCK_VEL);
  scene_add_body(scene, block);
  create_player_platform_collision(scene, ball, block);
  return scene;
}

void compute_new_positions(Scene *scene, double dt){
  scene_tick(scene, dt);
}

void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* ball = scene_get_body(scene, 1);
  if (type == KEY_PRESSED) {
    switch(key) {
          case LEFT_ARROW:
              body_set_velocity(ball, vec_add(body_get_velocity(ball), vec_negate(SPEED)));
              break;
          case RIGHT_ARROW:
              body_set_velocity(ball, vec_add(body_get_velocity(ball), SPEED));
              break;
          case ' ':
              body_set_velocity(ball, vec_add(body_get_velocity(ball), SPEED_UP));
              break;
      }
  }
}

int main(int argc, char *argv[]){
  srand(time(0));
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  Scene *scene = scene_init();
  Body *gravity_body = get_gravity_body();
  scene_add_body(scene, gravity_body);
  init_scene(scene);
  sdl_on_key(on_key, scene);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    compute_new_positions(scene, dt);
    sdl_clear();
    scene_draw(scene);
    sdl_show();
  }
  scene_free(scene);
  return 0;
}

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
  .x = 500.0,
  .y = 250.0
};

const int NUM_ROWS = 4;
const double RIGHT = 0;
const double LEFT = M_PI;
const Vector SPD_LEFT = (Vector){-500, 0};
const Vector SPD_RIGHT = (Vector){500, 0};
const Vector BALL_POS = (Vector){0, 0};
const Vector BALL_VEL = (Vector){50, 200};
const double BALL_MASS = 10;
const double BALL_RADIUS = 7;
const RGBColor BALL_COLOR = (RGBColor){0.95, 0.0, 0.0};
const RGBColor PADDLE_COLOR = (RGBColor){0.95, 0.0, 0.0};
const Vector PADDLE_DIM = (Vector){80, 15};
const Vector BLOCK_DIM = (Vector){90, 30};
const double BLOCK_SPACING = 7;
const double COLOR_FREQ = 0.5;
const RGBColor WHITE = (RGBColor){1.0, 1.0, 1.0};

typedef enum {
    BALL,
    WALL,
    BLOCK,
    PADDLE
} BodyType;

BodyType get_type(Body *body) {
    return *(BodyType *) body_get_info(body);
}

RGBColor rainbow(double seed){
  seed *= COLOR_FREQ;
  return (RGBColor){(1 + sin(seed))/2.0, (1 + sin(seed + 2))/2.0, (1+sin(seed + 4))/2.0};
}

Body *init_block(Vector position, Vector dimension, RGBColor color){
  List *block = list_init(4, free);
  BodyType *type = malloc(sizeof(*type));
  *type = BLOCK;
  list_add(block, vec_init((Vector){dimension.x / 2.0, dimension.y / 2.0}));
  list_add(block, vec_init((Vector){dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, dimension.y / 2.0}));
  polygon_translate(block, position);
  return body_init_with_info(block, INFINITY, color, type, free);
}

Body *init_paddle(Vector position, Vector dimension, RGBColor color){
  List *block = list_init(4, free);
  BodyType *type = malloc(sizeof(*type));
  *type = PADDLE;
  list_add(block, vec_init((Vector){dimension.x / 2.0, dimension.y / 2.0}));
  list_add(block, vec_init((Vector){dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, dimension.y / 2.0}));
  polygon_translate(block, position);
  return body_init_with_info(block, INFINITY, color, type, free);
}

Body *init_wall(Vector position, Vector dimension, RGBColor color){
  List *block = list_init(4, free);
  BodyType *type = malloc(sizeof(*type));
  *type = WALL;
  list_add(block, vec_init((Vector){dimension.x / 2.0, dimension.y / 2.0}));
  list_add(block, vec_init((Vector){dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, dimension.y / 2.0}));
  polygon_translate(block, position);
  return body_init_with_info(block, INFINITY, color, type, free);
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

void init_scene_boundaries(Scene *scene, Body *ball) {
  Vector dim1 = (Vector){BLOCK_SPACING, 2 * BOUNDARY.y};
  Body *leftBound = init_wall((Vector){-BOUNDARY.x - BLOCK_SPACING / 2.0, 0.0}, dim1, WHITE);
  Body *rightBound = init_wall((Vector){BOUNDARY.x + BLOCK_SPACING / 2.0, 0.0}, dim1, WHITE);
  Vector dim2 = (Vector){2 * BOUNDARY.x, BLOCK_SPACING};
  Body *topBound = init_wall((Vector){0.0, BOUNDARY.y + BLOCK_SPACING / 2.0}, dim2, WHITE);
  Body *botBound = init_wall((Vector){0.0, -BOUNDARY.y - 2 * BALL_RADIUS}, dim2, WHITE);
  create_physics_collision(scene, 1.0, ball, leftBound);
  create_physics_collision(scene, 1.0, ball, rightBound);
  create_physics_collision(scene, 1.0, ball, topBound);
  create_destructive_collision(scene, ball, botBound);
  scene_add_body(scene, leftBound);
  scene_add_body(scene, rightBound);
  scene_add_body(scene, topBound);
  scene_add_body(scene, botBound);
}

Scene *init_scene(void){
  Scene *scene = scene_init();
  Body *paddle = init_paddle((Vector){0.0, PADDLE_DIM.y + BLOCK_SPACING - BOUNDARY.y}, PADDLE_DIM, PADDLE_COLOR);
  Body *ball = init_ball(BALL_POS, BALL_MASS, BALL_RADIUS, BALL_COLOR);
  body_set_velocity(ball, BALL_VEL);
  create_physics_collision(scene, 1.05, ball, paddle);
  scene_add_body(scene, paddle);
  scene_add_body(scene, ball);
  int numBlocks = (int)floor(2 * BOUNDARY.x / (BLOCK_DIM.x + BLOCK_SPACING));
  double offset = (2 * BOUNDARY.x - numBlocks * (BLOCK_DIM.x + BLOCK_SPACING)) / 2.0;
  double y = BOUNDARY.y - BLOCK_SPACING - BLOCK_DIM.y / 2.0;
  for(int i = 0; i < NUM_ROWS; i++){
    double x = -BOUNDARY.x + offset + BLOCK_DIM.x / 2.0;
    for(double j = 0; j < numBlocks; j++){
      Body *block = init_block((Vector){x, y}, BLOCK_DIM, rainbow(x));
      scene_add_body(scene, block);
      create_partial_collision(scene, 1.0, ball, block);
      x += BLOCK_DIM.x + BLOCK_SPACING;
    }
    y -= BLOCK_DIM.y + BLOCK_SPACING;
  }
  init_scene_boundaries(scene, ball);
  return scene;
}

bool check_game_over(Scene* scene){
  size_t num_bricks = 0;
  for(size_t i = 1; i < scene_bodies(scene); i++) {
    Body *body = scene_get_body(scene, i);
    if(get_type(body) == BLOCK){
      num_bricks++;
    }
  }
  return num_bricks == 0;
}

int check_bricks(Scene* scene){
  size_t num_bricks = 0;
  for(size_t i = 1; i < scene_bodies(scene); i++) {
    Body *body = scene_get_body(scene, i);
    if(get_type(body) == BLOCK){
      num_bricks++;
    }
  }
  return num_bricks;
}

void add_ball(Scene* scene){
  Body *ball = init_ball(BALL_POS, BALL_MASS, BALL_RADIUS, BALL_COLOR);
  body_set_velocity(ball, BALL_VEL);
  create_physics_collision(scene, 1.05, ball, scene_get_body(scene, 0));
  scene_add_body(scene, ball);
  for(size_t i = 1; i < scene_bodies(scene); i++) {
    Body *body = scene_get_body(scene, i);
    if(get_type(body) == BLOCK){
      create_partial_collision(scene, 1.0, ball, body);
    }
    if(get_type(body) == BALL){
      create_physics_collision(scene, 1.0, ball, body);
    }
  }
}

void compute_new_positions(Scene *scene, double dt, int initial){
  scene_tick(scene, dt);
  if((initial - check_bricks(scene) % 5) == 0){
    add_ball(scene);
  }
}

void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* paddle = scene_get_body(scene, 0);
  if (type == KEY_PRESSED) {
    switch(key) {
          case LEFT_ARROW:
              body_set_velocity(paddle, SPD_LEFT);
              break;
          case RIGHT_ARROW:
              body_set_velocity(paddle, SPD_RIGHT);
              break;
      }
    }
    if(type == KEY_RELEASED)
    {
      body_set_velocity(paddle, VEC_ZERO);
    }
}

int main(int argc, char *argv[]){
  srand(time(0));
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  Scene *scene = init_scene();
  int num_bricks = check_bricks(scene);
  sdl_on_key(on_key, scene);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    compute_new_positions(scene, dt, num_bricks);
    if(check_game_over(scene)){
      return 1;
    }
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

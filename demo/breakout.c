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
#include "collision.h"

const Vector BOUNDARY = {
  .x = 500.0,
  .y = 250.0
};
const int NUM_ROWS = 3;
const double RIGHT = 0;
const double LEFT = M_PI;
const Vector SPD_LEFT = (Vector){-200, 0};
const Vector SPD_RIGHT = (Vector){200, 0};
const Vector BALL_POS = VEC_ZERO;
const Vector BALL_VEL = (Vector){0.0, -200};
const double BALL_MASS = 10;
const double BALL_RADIUS = 5;
const RGBColor BALL_COLOR = (RGBColor){0.95, 0.0, 0.0};
const RGBColor PADDLE_COLOR = (RGBColor){0.95, 0.0, 0.0};
const Vector PADDLE_DIM = (Vector){20, 5};
const Vector BLOCK_DIM = (Vector){30, 10};
const double BLOCK_SPACING = 3;
const double COLOR_FREQ = 0.5;

typedef struct info {
  size_t tag;
  bool isShooter;
} Info;

Vector *init_vec(Vector v){
  Vector *vector = malloc(sizeof(v));
  vector->x = v.x;
  vector->y = v.y;
  return vector;
}
Info *init_info(Info i){
  Info *info = malloc(sizeof(i));
  info->tag = i.tag;
  info->isShooter = i.isShooter;
  return info;
}
RGBColor rainbow(double seed){
  seed *= COLOR_FREQ;
  return (RGBColor){(1 + sin(seed))/2.0, (1 + sin(seed + 2))/2.0, (1+sin(seed + 4))/2.0};
}

Body *init_block(Vector position, Vector dimension, RGBColor color){
  List *block = list_init(4, free);
  list_add(block, init_vec(dimension.x / 2.0, dimension.y / 2.0));
  list_add(block, init_vec(-dimension.x / 2.0, dimension.y / 2.0));
  list_add(block, init_vec(dimension.x / 2.0, -dimension.y / 2.0));
  list_add(block, init_vec(-dimension.x / 2.0, -dimension.y / 2.0));
  polygon_translate(block, position);
  return body_init(block, INFINITY, color);
}

Body *init_ball(Vector position, double mass, double radius, RGBColor color){
  List *ball = list_init(75, free);
  for(double angle = 0.0; angle < 2 * M_PI; angle += 0.05){
    list_add(ball, init_vec(vec_multiply(radius, (Vector){cos(angle), sin(angle)})));
  }
  polygon_translate(ball, position);
  return body_init(ball, mass, color);
}

void *init_scene_boundaries(Scene *scene, Body *ball){
  Vector dim1 = (Vector){BLOCK_SPACING, BOUNDARY.y};
  Body *leftBound = init_block((Vector)(-BOUNDARY.x - BLOCK_SPACING / 2.0, 0.0), dim1, BALL_COLOR);
  Body *rightBound = init_block((Vector)(BOUNDARY.x + BLOCK_SPACING / 2.0, 0.0), dim1, BALL_COLOR);
  Vector dim2 = (Vector){BOUNDARY.x, BLOCK_SPACING.y};
  Body *topBound = init_block((Vector)(0.0, BOUNDARY.y + BLOCK_SPACING / 2.0), dim2, BALL_COLOR);
  Body *botBound = init_block((Vector)(0.0, -BOUNDARY.y - BLOCK_SPACING / 2.0), dim2, BALL_COLOR);
  create_physics_collision(scene, 1.0, ball, leftBound);
  create_physics_collision(scene, 1.0, ball, rightBound);
  create_physics_collision(scene, 1.0, ball, topBound);
  create_physics_collision(scene, 1.0, ball, botBound);
  scene_add_body(scene, leftBound);
  scene_add_body(scene, rightBound);
  scene_add_body(scene, topBound);
  scene_add_body(scene, botBound);
}

Scene *init_scene(void){
  Scene *scene = scene_init();
  Body *paddle = init_block((Vector){0.0, PADDLE_DIM.y + BLOCK_SPACING - BOUNDARY.y}, PADDLE_DIM, PADDLE_COLOR);
  Body *ball = init_ball(BALL_POS, BALL_MASS, BALL_RADIUS, BALL_COLOR);
  body_set_velocity(ball, BALL_VEL);
  create_physics_collision(scene, 1.0, ball, paddle);
  scene_add_body(scene, paddle);
  scene_add_body(scene, ball);
  int numBlocks = (int)floor(BOUNDARY.x / (BLOCK_DIM.x + BLOCK_SPACING));
  double offset = (BOUNDARY.x - numBlocks * (BLOCK_DIM.x + BLOCK_SPACING)) / 2.0;
  double y = BOUNDARY.y - BLOCK_SPACING - BLOCK_DIM.y / 2.0;
  for(int i = 0; i < NUM_ROWS; i++){
    double x = -BOUNDARY.x + offset + BLOCK_DIM.x / 2.0;
    for(double j = 0; j < numBlocks; j++){
      Body *block = init_block((Vector){x, y}, BLOCK_DIM, rainbow(x));
      scene_add_body(scene, block);
      create_partial_collision(scene, 1.0, ball, block);
      x += BLOCK_DIM.x + BLOCK_SPACING;
    }
    y += BLOCK_DIM.y + BLOCK_SPACING;
  }
  init_scene_boundaries(scene, ball);
  return scene;
}

bool check_game_over(Scene* scene){
  size_t num_enemies = 0;
  for(size_t i = 1; i < scene_bodies(scene); i++) {
    Body *body = scene_get_body(scene, i);
    Info* info = body_get_info(body);
    if(info->tag == 2){
      num_enemies++;
    }
  }
  return num_enemies == 0;
}

bool compute_new_positions(Scene *scene, double dt){
  scene_tick(scene, dt);
  return true;
}

void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* paddle = scene_get_body(scene, 0);
  Body* bullet;
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
  sdl_on_key(on_key, scene);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    if(compute_new_positions(scene, dt))
    {
      // if(check_game_over(scene)){
      //   puts("You win! :-)\n");
      //   return 0;
      // }
      // puts("Game over\n");
      // return 0;
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

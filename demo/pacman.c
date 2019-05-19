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

const Vector BOUNDARY = {
  .x = 500.0,
  .y = 250.0
};
const double RIGHT = 0;
const double LEFT = M_PI;
const double UP = M_PI /2.0;
const double DOWN = 3.0 * M_PI / 2.0;
const double PACMAN_RADIUS = 30;
const double FOOD_RADIUS = 5;
const size_t NUM_FOOD = 15;
const RGBColor YELLOW = (RGBColor){0.95, 0.9, 0.0};
const Vector DIR_RIGHT = (Vector){2, 0};
const Vector DIR_UP = (Vector){0, 2};
const Vector DIR_LEFT = (Vector){-2, 0};
const Vector DIR_DOWN = (Vector){0, -2};

Body *pacman;

Vector *init_vec(Vector v){
  Vector *vector = malloc(sizeof(v));
  vector->x = v.x;
  vector->y = v.y;
  return vector;
}

Body *init_pacman(Vector position){
  List *pacman = list_init(50, free);
  for(double angle = M_PI / 4.0; angle < 7.0 / 4.0 * M_PI; angle += 0.05){
    list_add(pacman, init_vec(vec_multiply(PACMAN_RADIUS, (Vector){cos(angle), sin(angle)})));
  }
  list_add(pacman, init_vec((Vector){0.0, 0.0}));
  polygon_translate(pacman, position);
  return body_init(pacman, 0.0, YELLOW);
}

Body *init_food(Vector position){
  List *food = list_init(75, free);
  for(double angle = 0.0; angle < 2 * M_PI; angle += 0.05){
    list_add(food, init_vec(vec_multiply(FOOD_RADIUS, (Vector){cos(angle), sin(angle)})));
  }
  polygon_translate(food, position);
  return body_init(food, 0.0, YELLOW);
}

int randomValue(int min, int max){
  return (rand()) % (max - min + 1) + min;
}

Scene *init_scene(void){
  Scene *scene = scene_init();
  scene_add_body(scene, init_pacman((Vector){0, 0}));
  for(size_t i = 0; i < NUM_FOOD; i++){
    double x = (double)randomValue((int)-BOUNDARY.x, (int)BOUNDARY.x);
    double y = (double)randomValue((int)-BOUNDARY.y, (int)BOUNDARY.y);
    scene_add_body(scene, init_food((Vector){x, y}));
  }
  return scene;
}

void compute_new_positions(Scene *scene, double dt){
  scene_tick(scene, dt);
  body_wrap(pacman, BOUNDARY);
  Body * bod;
  for(size_t i = 1; i < scene_bodies(scene); i++)
  {
    bod = scene_get_body(scene, i);
    if(body_collision(pacman, bod)) {
        scene_remove_body(scene, i);
        double x = (double)randomValue((int)-BOUNDARY.x, (int)BOUNDARY.x);
        double y = (double)randomValue((int)-BOUNDARY.y, (int)BOUNDARY.y);
        scene_add_body(scene, init_food((Vector){x, y}));
    }
  }
}
void on_key(char key, KeyEventType type, double held_time) {
  if (type == KEY_PRESSED) {
    switch(key) {
          case UP_ARROW:
              puts("UP pressed");
              if(body_get_velocity(pacman).x != 0 || body_get_velocity(pacman).y < 0) {
                body_set_velocity(pacman, vec_multiply(4, DIR_UP));
              }
              body_accelerate(pacman, DIR_UP, held_time);
              body_set_rotation(pacman, UP);
              break;
          case DOWN_ARROW:
              puts("DOWN pressed");
              if(body_get_velocity(pacman).x != 0 || body_get_velocity(pacman).y > 0) {
                body_set_velocity(pacman, vec_multiply(4, DIR_DOWN));
              }
              body_accelerate(pacman, DIR_DOWN, held_time);
              body_set_rotation(pacman, DOWN);
              break;
          case LEFT_ARROW:
              puts("LEFT pressed");
              if(body_get_velocity(pacman).y != 0 || body_get_velocity(pacman).x > 0) {
                body_set_velocity(pacman, vec_multiply(4, DIR_LEFT));
              }
              body_accelerate(pacman, DIR_LEFT, held_time);
              body_set_rotation(pacman, LEFT);
              break;
          case RIGHT_ARROW:
              puts("RIGHT pressed");
              if(body_get_velocity(pacman).y != 0 || body_get_velocity(pacman).x < 0) {
                body_set_velocity(pacman, vec_multiply(4, DIR_RIGHT));
              }
              body_accelerate(pacman, DIR_RIGHT, held_time);
              body_set_rotation(pacman, RIGHT);
              break;
      }
    }
}

int main(int argc, char *argv[]){
  srand(time(0));
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  Scene *scene = init_scene();
  pacman = scene_get_body(scene, 0);
  sdl_on_key(on_key);
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

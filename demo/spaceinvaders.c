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
const int NROWS = 10;
const int NCOLS = 3;
const RGBColor YELLOW = (RGBColor){0.95, 0.9, 0.0};
const RGBColor RED = (RGBColor){0.95, 0.0, 0.0};
const RGBColor GREY = (RGBColor){0.25, 0.25, 0.25};
const RGBColor GREEN = (RGBColor){0.0, 0.95, 0.0};
const int ALIEN_RADIUS = 20;
const int SHIP_RADIUS = 19;
const double RIGHT = 0;
const double LEFT = M_PI;
const Vector SPD_LEFT = (Vector){-200, 0};
const Vector SPD_RIGHT = (Vector){200, 0};
const Vector SHOT_OFFSET = (Vector){0, 20};
const Vector SHOT_SPEED = (Vector){0, 200};
const int SHOOTER_FREQ = 1;
const int SHOT_FREQ = 2;

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
Body *init_alien(Vector position, bool isShooter){
  List *alien = list_init(50, free);
  for(double angle = - 0.5 * M_PI / 4.0; angle < 4.5 / 4.0 * M_PI; angle += 0.05){
    list_add(alien, init_vec(vec_multiply(ALIEN_RADIUS, (Vector){cos(angle), sin(angle)})));
  }
  list_add(alien, init_vec((Vector){0.0, 0.0}));
  polygon_translate(alien, position);
  Body *bod = body_init_with_info(alien, 10, GREY, init_info((Info){2, isShooter}), free);
  body_set_velocity(bod, (Vector){100, 0});
  return bod;
}

Body *init_ship(Vector position){
  List *ship = list_init(75, free);
  for(double angle = 0.0; angle < 2 * M_PI; angle += 0.05){
    list_add(ship, init_vec(vec_multiply(SHIP_RADIUS, (Vector){cos(angle), sin(angle)})));
  }
  polygon_translate(ship, position);
  return body_init_with_info(ship, 20, GREEN, init_info((Info){1, true}), free);
}

Body *init_bullet(Vector position, Vector velocity, RGBColor color){
  List * bullet = list_init(5, free);
  list_add(bullet, init_vec((Vector){6,0}));
  list_add(bullet, init_vec((Vector){6,12}));
  list_add(bullet, init_vec((Vector){0,12}));
  list_add(bullet, init_vec((Vector){0,0}));
  polygon_translate(bullet, position);
  Body * bod = body_init_with_info(bullet, 10, color, init_info((Info){3, false}), free);
  body_set_velocity(bod, velocity);
  return bod;

}
/* Generates a random value in range (min, max) . Returns an int.*/
int randomValue(int min, int max){
    if(rand() % 2 == 1)
        return rand() % (max - min + 1) + min;
    return -1 * rand() % (max - min + 1) + min;
}

void add_collision(Scene *scene, Body *body, size_t tag){
  Body *otherBod;
  for(size_t i = 0; i < scene_bodies(scene); i++) {
      otherBod = scene_get_body(scene, i);
      Info* info = body_get_info(otherBod);
      if(info->tag == tag || info->tag == 3){
        create_partial_destructive_collision(scene, body, otherBod);
        //create_destructive_collision(scene, body, otherBod);
      }
  }
}

void add_collision_ship(Scene *scene, Body *body){
  add_collision(scene, body, 1);
}

void add_collision_alien(Scene *scene, Body *body){
  add_collision(scene, body, 2);
}


Scene *init_scene(void){
  Scene *scene = scene_init();
  scene_add_body(scene, init_ship((Vector){0, -BOUNDARY.y + 40}));
  for(int x = - NROWS / 2; x < NROWS / 2; x += 1){
    for(int y = 0; y < NCOLS; y ++){
      bool shooter = randomValue(1, 10) <= SHOOTER_FREQ;
      scene_add_body(scene, init_alien((Vector){(2 * x * BOUNDARY.x / NROWS), ((y + 3) * BOUNDARY.y / (2 * NCOLS))}, shooter));
    }
  }
  for(size_t i = 1; i < scene_bodies(scene); i++){
    Body *otherBod = scene_get_body(scene, i);
    add_collision_ship(scene, otherBod);
  }
  return scene;
}

void random_shoot(Scene *scene){
  for(size_t i = 1; i < scene_bodies(scene); i++) {
    Body *body = scene_get_body(scene, i);
    Info* info = body_get_info(body);
    if(info->isShooter && randomValue(1, 1000) == SHOT_FREQ)
    {
      Body *bullet = init_bullet(vec_subtract(body_get_centroid(body), SHOT_OFFSET), vec_negate(SHOT_SPEED), GREY);
      add_collision_ship(scene, bullet);
      scene_add_body(scene, bullet);
    }
  }
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
  for(size_t i = 0; i < scene_bodies(scene); i++)
  {
    Body *bod = scene_get_body(scene, i);
    Info* info = body_get_info(bod);
    if(info->tag == 2){
      alien_wrap(bod, BOUNDARY);
    }
    if(info->tag == 1){
      player_wrap(bod, BOUNDARY);
    }
  }
  random_shoot(scene);
  scene_tick(scene, dt);
  Info* info = body_get_info(scene_get_body(scene, 0));
  return info->tag != 1;
}

void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* ship = scene_get_body(scene, 0);
  Body* bullet;
  if (type == KEY_PRESSED) {
    switch(key) {
          case LEFT_ARROW:
              body_set_velocity(ship, SPD_LEFT);
              break;
          case RIGHT_ARROW:
              body_set_velocity(ship, SPD_RIGHT);
              break;
          case ' ':
              bullet = init_bullet(vec_add(body_get_centroid(ship), SHOT_OFFSET), SHOT_SPEED, GREEN);
              add_collision_alien(scene, bullet);
              scene_add_body(scene, bullet);
              break;
      }
    }
    if(type == KEY_RELEASED)
    {
      body_set_velocity(ship, (Vector){0,0});
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
      if(check_game_over(scene)){
        puts("You win! :-)\n");
        return 0;
      }
      puts("Game over\n");
      return 0;
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

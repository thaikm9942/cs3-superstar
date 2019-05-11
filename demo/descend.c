#include "sdl_wrapper.h"
#include "forces_game.h"
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
#include <math.h>
#include "status.h"
const Vector BOUNDARY = {
  .x = 100.0,
  .y = 100.0
};

const int NUM_ROWS = 3;
const Vector IMPULSE_X = (Vector){2000, 0};
const Vector IMPULSE_UP = (Vector){0, 10000};
const Vector BALL_POS = (Vector){0, 10};
const Vector STAR_VEL = (Vector){0, -15};
const Vector BLOCK_VEL = (Vector){0, -10};
const double BALL_MASS = 200;
const double BALL_RADIUS = 10;
const RGBColor BALL_COLOR = (RGBColor){0.95, 0.0, 0.0};
const RGBColor PLATFORM_COLOR = (RGBColor){0.0, 0.0, 0.95};
const Vector BLOCK_DIM = (Vector){10, 2};
const double BLOCK_SPACING = 7;
const double COLOR_FREQ = 0.25;
const RGBColor WHITE = (RGBColor){1.0, 1.0, 1.0};
const RGBColor BLACK = (RGBColor){0.0, 0.0, 0.0};
const int NSTART_PLATFORMS = 6;
#define G 6.67E-11 // N m^2 / kg^2
#define G2 6.67E-3 // N m^2 / kg^2
#define M 6E24 // kg
#define g 9.8 // m / s^2
#define R (sqrt(G * M / g)) // m


// Global Variable to track points
// TODO see if can be made not global
int score;

RGBColor rainbow(double seed){
  seed *= COLOR_FREQ;
  return (RGBColor){(1 + sin(seed))/2.0, (1 + sin(seed + 2))/2.0, (1+sin(seed + 4))/2.0};
}

int randomValue(int min, int max){
    if(rand() % 2 == 1)
        return rand() % (max - min + 1) + min;
    return -1 * rand() % (max - min + 1) + min;

}

void add_spikes(Scene *scene)
{
  for(int i = -20; i < 20; i++){
    Body *spike = spike_init((Vector){i * 10.0, (-1 * BOUNDARY.y)}, 10.0, INFINITY, BLACK, 1);
    scene_add_body(scene, spike);
    Body* ball = scene_get_body(scene, 0);
    create_partial_destructive_collision_with_life(scene, spike, ball);
    for(size_t i = 0; i < scene_bodies(scene); i++){
      Body* body = scene_get_body(scene, i);
      BodyInfo* info = body_get_info(body);
      BodyType type = body_info_get_type(info);
      if(type == PLATFORM){
        create_partial_destructive_collision_with_life(scene, spike, body);
      }
    }
  }
}

/* Spawns a point on the last added platform on the screen aka the highest platform */
void add_point(Scene *scene)
{
  Body* ball = scene_get_body(scene, 0);
  for(size_t i = scene_bodies(scene) - 1; i > 0; i--){
    Body* body = scene_get_body(scene, i);
    BodyInfo* info = body_get_info(body);
    BodyType type = body_info_get_type(info);
    if(type == PLATFORM){
      // Spawn a point
      Body *point = point_init((Vector){body_get_centroid(body).x, body_get_centroid(body).y + 8},
       3.0, 20.0, BALL_COLOR, 1);
       scene_add_body(scene, point);
       body_set_velocity(point, BLOCK_VEL);
       // Creates collisions that destroy the point on collions. Scoring handled in
       // update method
       create_partial_destructive_collision(scene, ball, point);
       /* THis screws up scoring so we're ignoritn it for now
       // Link collisions with spikes
       for(size_t j = 0; j < scene_bodies(scene); j++){
         Body* body = scene_get_body(scene, j);
         BodyInfo* info = body_get_info(body);
         BodyType type = body_info_get_type(info);
         if(type == SPIKE){
           create_partial_destructive_collision_with_life(scene, body, point);
         }
       }*/
       break;

    }
  }
  //Body *point = point_init((Vector){randomValue(0, BOUNDARY.x), randomValue(0, BOUNDARY.y)}, 3.0, 20.0, BALL_COLOR, 1);

}

void add_platform_altitude(Scene *scene, int y)
{
  // Top of screeen is Dimension.y, so make new platforms appear there.
  Body *platform = block_init((Vector){randomValue(0, BOUNDARY.x), y}, (Vector){30, 5}, PLATFORM_COLOR, 1);
  body_set_velocity(platform, BLOCK_VEL);
  scene_add_body(scene, platform);
  Body* player = scene_get_body(scene, 0);
  create_player_platform_collision(scene, player, platform);
}

void add_platform_first(Scene *scene)
{
  // Top of screeen is Dimension.y, so make new platforms appear there.
  Body *platform = block_init((Vector){BALL_POS.x, BALL_POS.y - 2 * BALL_RADIUS}, (Vector){30, 5}, PLATFORM_COLOR, 1);
  body_set_velocity(platform, BLOCK_VEL);
  scene_add_body(scene, platform);
  Body* player = scene_get_body(scene, 0);
  create_player_platform_collision(scene, player, platform);
}

Scene *init_scene(Scene *scene){
  Body *player = player_init(5, BALL_POS, BALL_RADIUS, BALL_MASS, BALL_COLOR, 3);
  scene_add_body(scene, player);
  create_gravity(scene, player);
  add_platform_first(scene);
  for(size_t i = 0; i < NSTART_PLATFORMS; i ++)
  {
    add_platform_altitude(scene, randomValue(BOUNDARY.y * i / NSTART_PLATFORMS, BOUNDARY.y * (i + 1) / NSTART_PLATFORMS));
  }
  add_spikes(scene);
  return scene;
}

void add_platform(Scene *scene)
{
  // Top of screeen is Dimension.y, so make new platforms appear there.
  Body *platform = block_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y}, (Vector){30, 5}, PLATFORM_COLOR, 1);
  body_set_velocity(platform, BLOCK_VEL);
  scene_add_body(scene, platform);
  Body* player = scene_get_body(scene, 0);
  create_player_platform_collision(scene, player, platform);
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body* body = scene_get_body(scene, i);
    BodyInfo* info = body_get_info(body);
    BodyType type = body_info_get_type(info);
    if(type == SPIKE){
      create_partial_destructive_collision_with_life(scene, body, platform);
    }
  }
}


// Return 0 if game running, return -1 if game over
int compute_new_positions(Scene *scene, double dt){
  int cnt = 0;
  if(rand() % 800 == 4)
  {
    add_platform(scene);
  }
  if(rand() % 4800 == 4)
  {
    add_point(scene);
  }
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body* body = scene_get_body(scene, i);
    BodyInfo* info = body_get_info(body);
    BodyType type = body_info_get_type(info);
    if(type == POINT){
      cnt ++;
    }
  }
  Body* ball = scene_get_body(scene, 0);
  if(body_info_get_type(body_get_info(ball)) != PLAYER)
    return -1;
  scene_tick(scene, dt);
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body* body = scene_get_body(scene, i);
    BodyInfo* info = body_get_info(body);
    BodyType type = body_info_get_type(info);
    if(type == POINT){
      cnt --;
    }
  }
  score += cnt;
  if(cnt != 0){
    printf("%i\n", score);
  }
  return 0;
}

void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* ball = scene_get_body(scene, 0);
  if (type == KEY_PRESSED) {
    switch(key) {
          case LEFT_ARROW:
              body_add_impulse(ball, vec_negate(IMPULSE_X));
              break;
          case RIGHT_ARROW:
              body_add_impulse(ball, IMPULSE_X);
              break;
          /*case UP_ARROW:
              //if(on_platform(ball))
              //jump
              //Otherwise do nothing
              //body_set_velocity(ball, vec_add(body_get_velocity(ball), SPEED_UP));
              body_add_impulse(ball, IMPULSE_UP);
              break;
          case DOWN_ARROW:
              body_set_velocity(ball, vec_add(body_get_velocity(ball), SPEED_DOWN));
              break;*/
          case ' ':
          /*
              printf("lol\n");
              BodyInfo* info = body_get_info(ball);
              bool colliding = body_info_get_collision(info);
              */
              body_add_impulse(ball, IMPULSE_UP);

      }
    }
    if(type == KEY_RELEASED)
    {
      //body_set_velocity(ball, STAR_VEL);
      Vector vec = body_get_velocity(ball);
      vec.x = 0;
      body_set_velocity(ball, vec);
    }
}
void draw(Scene * scene, int frame)
{
    size_t j = 0;
    Body* star = scene_get_body(scene, 0);
    if((scene_get_status(scene)->isInvincible) && (frame % 5 == 0 || frame % 5 == 1 || frame % 5 == 2)){
      List *polygon = body_get_shape(star);
      sdl_draw_polygon(polygon, BLACK);
      j++;
    }
    for(size_t i = j; i < scene_bodies(scene); i++)
    {
      Body *body = scene_get_body(scene, i);
      List *polygon = body_get_shape(body);
      sdl_draw_polygon(polygon, body_get_color(body));
    }

}
int main(int argc, char *argv[]){
  int frame = 0;
  srand(time(0));
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  Scene *scene = scene_init();
  init_scene(scene);
  sdl_on_key(on_key, scene);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    if(compute_new_positions(scene, dt) == -1)
    {
      break;
    }
    sdl_clear();
    draw(scene, frame);
    frame ++;
    sdl_show();
  }
  scene_free(scene);
  return 0;
}

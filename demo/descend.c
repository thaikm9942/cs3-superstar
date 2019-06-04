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

const Vector BOUNDARY = {
  .x = 200.0,
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
    create_partial_collision_with_life(scene, 1, spike, ball);
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
void add_point(Scene *scene) {
  Body* ball = scene_get_body(scene, 0);
  for(size_t i = scene_bodies(scene) - 1; i > 0; i--) {
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
      // the CollisionHandler
      create_player_point_collision(scene, ball, point);
      for(size_t j = 0; j < scene_bodies(scene); j++){
        Body* spike = scene_get_body(scene, j);
        BodyInfo* spike_info = body_get_info(spike);
        BodyType spike_type = body_info_get_type(spike_info);
        if(spike_type == SPIKE){
          create_partial_destructive_collision_with_life(scene, spike, point);
        }
      }
      break;
    }
  }
}

void add_platform_altitude(Scene *scene, int y) {
  // Top of screen is Dimension.y, so make new platforms appear there.
  Body *platform = block_init((Vector){randomValue(0, BOUNDARY.x), y}, (Vector){30, 5}, PLATFORM_COLOR, 1);
  body_set_velocity(platform, BLOCK_VEL);
  scene_add_body(scene, platform);
  Body* player = scene_get_body(scene, 0);
  create_player_platform_collision(scene, player, platform);
}

void add_platform_first(Scene *scene){
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
int step(Scene *scene, double dt){
  if(rand() % 200 == 4){
    add_platform(scene);
  }
  if(rand() % 200 == 4){
    add_point(scene);
  }
  Body* body = scene_get_body(scene, 0);
  if(body_info_get_type(body_get_info(body)) != PLAYER){
    return -1;
  }
  scene_tick(scene, dt);
  return 0;
}

// KeyHandler
void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* player = scene_get_body(scene, 0);
  BodyInfo* info = body_get_info(player);
  bool colliding = body_info_get_collision(info);
  if (type == KEY_PRESSED) {
    switch(key) {
          case LEFT_ARROW:
              body_add_impulse(player, vec_negate(IMPULSE_X));
              break;
          case RIGHT_ARROW:
              body_add_impulse(player, IMPULSE_X);
              break;
          case ' ':
              if(colliding){
                printf("collision\n");
                body_add_impulse(player, IMPULSE_UP);
                break;
              }
              else{
                printf("no collision\n");
              }
              break;
      }
    }
    if(type == KEY_RELEASED)
    {
      Vector vec = body_get_velocity(player);
      vec.x = 0;
      body_set_velocity(player, vec);
    }
}

// Draws the scene
void draw(Scene *scene, int frame){
    for(size_t i = 0; i < scene_bodies(scene); i++)
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
  char* displayScore = (char *)malloc(sizeof(char)*10);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    if(step(scene, dt) == -1)
    {
      break;
    }
    sdl_clear();
    sprintf(displayScore, "Score: %zu", scene_get_score(scene));
    drawText(displayScore,27,(RGBColor){0,100,255}, (Vector){20,0});
    draw(scene, frame);
    frame++;
    sdl_show();
  }
  free(displayScore);
  scene_free(scene);
  return 0;
}

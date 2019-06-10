#include "sdl_wrapper.h"
#include "forces_game.h"
#include "collision.h"
#include "scene.h"
#include "hazard.h"
#include "powerup.h"
#include "body.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

const Vector BOUNDARY = {
  .x = 200.0,
  .y = 100.0
};

const int NUM_ROWS = 3;
const Vector IMPULSE_X = (Vector){5000, 0};
const Vector IMPULSE_UP = (Vector){0, 17500};
const Vector BALL_POS = (Vector){0, 10};
const Vector STAR_VEL = (Vector){0, -15};
const Vector DEFAULT_VEL = (Vector){0, -10};
const Vector MAX_VEL = (Vector){50, 250};
const double BALL_MASS = 200;
const double BALL_RADIUS = 10;

const RGBColor PLATFORM_COLOR = (RGBColor){0.0, 0.0, 0.95};
const Vector BLOCK_DIM = (Vector){10, 2};
const double BLOCK_SPACING = 7;
const double COLOR_FREQ = 0.25;
const RGBColor RED = (RGBColor){0.95, 0.0, 0.0};
const RGBColor WHITE = (RGBColor){1.0, 1.0, 1.0};
const RGBColor YELLOW = (RGBColor){0.95, 0.95, 0.0};
const int NSTART_PLATFORMS = 6;
const int PLATFORM_DIST = 10;
#define M 6E26 // kg
#define g 9.8 // m / s^2
#define R (sqrt(G * M / g)) // m

int randomValue(int min, int max){
  if(rand() % 2 == 1) {
    return rand() % (max - min + 1) + min;
  }
  return -1 * rand() % (max - min + 1) + min;
}

void add_spikes(Scene *scene) {
  for(int i = -20; i < 20; i++){
    spike_hazard_init((Vector){i * 10.0, (-1 * BOUNDARY.y)}, scene);
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
      3.0, 20.0, RED, 1);
      scene_add_body(scene, point);
      body_set_velocity(point, DEFAULT_VEL);
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
void add_platform_physics(Scene *scene, Body *platform){
  for(size_t i = 0; i < scene_bodies(scene); i++){
      Body* body = scene_get_body(scene, i);
      BodyInfo* info = body_get_info(body);
      BodyType type = body_info_get_type(info);
      if(type == PLAYER){
          create_player_platform_collision(scene, body, platform);
      }
      if(type == SPIKE){
          create_partial_destructive_collision_with_life(scene, body, platform);
      }
  }
}

void add_platform_altitude(Scene *scene, int y, bool trigger) {
  // Top of screen is Dimension.y, so make new platforms appear there.
  Body *platform = block_init((Vector){randomValue(0, BOUNDARY.x), y}, (Vector){30, 5}, PLATFORM_COLOR, 1, trigger);
  body_set_velocity(platform, DEFAULT_VEL);
  add_platform_physics(scene, platform);
  scene_add_body(scene, platform);

}

void add_platform_first(Scene *scene){
  Body *platform = block_init((Vector){BALL_POS.x, BALL_POS.y - 2 * BALL_RADIUS}, (Vector){30, 5}, PLATFORM_COLOR, 1, false);
  body_set_velocity(platform, DEFAULT_VEL);
  scene_add_body(scene, platform);
  add_platform_physics(scene, platform);
}

void add_fair_platforms(Scene *scene){
  bool trigger = true;
  for(double i = -BOUNDARY.y; i < BOUNDARY.y; i += PLATFORM_DIST){
    add_platform_altitude(scene, i + 2 * BOUNDARY.y + (double) randomValue(-5, 5), trigger);
    trigger = false;
  }
}

void add_boundary(Scene *scene){
  Body *right = boundary_init((Vector){BOUNDARY.x + 15, 0}, (Vector){5, BOUNDARY.y * 2}, (RGBColor){0.0, 0.0, 0.0}, INFINITY);
  Body *left = boundary_init((Vector){-BOUNDARY.x - 15, 0}, (Vector){5, BOUNDARY.y * 2}, (RGBColor){0.0, 0.0, 0.0}, INFINITY);
  scene_add_body(scene, right);
  scene_add_body(scene, left);
}

void add_gravity_hazard(Scene *scene){
  gravity_hazard_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y}, scene);
}

void add_ball_hazard(Scene *scene){
  moving_ball_hazard_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y}, (Vector) {randomValue(0, MAX_VEL.x/4), DEFAULT_VEL.y}, randomValue(1, 10) * BALL_MASS, scene);
}

void add_star_invincibility(Scene *scene){
  Body *power = invincibility_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y}, 4.0, 12.0, YELLOW);
  body_set_velocity(power, DEFAULT_VEL);
  create_player_powerup_collision(scene, scene_get_body(scene, 0), power);
  scene_add_body(scene, power);
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body* body = scene_get_body(scene, i);
    BodyInfo* info = body_get_info(body);
    BodyType type = body_info_get_type(info);
    if(type == SPIKE){
      create_partial_destructive_collision_with_life(scene, body, power);
    }
  }
}

Scene *init_scene(Scene *scene){
  Body *player = player_init(5, BALL_POS, BALL_RADIUS, BALL_MASS, RED, 3);
  scene_add_body(scene, player);
  add_spikes(scene);
  add_boundary(scene);
  create_gravity(scene, player);
  add_platform_first(scene);
  for(size_t i = 0; i < NSTART_PLATFORMS; i ++)
  {
    add_platform_altitude(scene, randomValue(BOUNDARY.y * i / NSTART_PLATFORMS, BOUNDARY.y * (i + 1) / NSTART_PLATFORMS), false);
  }
  add_fair_platforms(scene);
  //Experimental
  //add_gravity_hazard(scene);
  //add_ball_hazard(scene);
  return scene;
}

int next_platforms(Scene *scene){
  for(size_t i = 1; i < scene_bodies(scene); i++){
    //printf("%d\n",body_info_get_type(body_get_info(scene_get_body(scene, i))));
    if (body_info_get_type(body_get_info(scene_get_body(scene, i))) == PLATFORM_TRIGGER){
      return 0;
    }
  }
  return 1;
}

// Return 0 if game running, return -1 if game over
int step(Scene *scene, double dt){
  //200
   if(rand() % 20 == 1){
     add_point(scene);
   }
   //1000
   if(rand() % 100 == 2){
     add_star_invincibility(scene);
   }
   //700
   if(rand() % 70 == 3){
     //add_gravity_hazard(scene);
   }
   //400
   // if(rand() % 40 == 4){
   //   add_ball_hazard(scene);
   // }

  if(next_platforms(scene)){
    add_fair_platforms(scene);
  }
  Body* body = scene_get_body(scene, 0);
  BodyInfo* info = body_get_info(body);
  size_t life = body_info_get_life(info);
  player_wrap(body, BOUNDARY);
  modulate_velocity(body);
  if(scene_get_status(scene)->isInvincible){
    body_set_color(body, YELLOW);
    if(life < 10000){
      body_info_set_life(info, life + 10000);
    }
  }
  else if(life > 10){
    body_set_color(body, RED);
    body_info_set_life(info, 3);
  }
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
            if(!(body_get_velocity(player).x < 0 - MAX_VEL.x)){
              body_add_impulse(player, vec_negate(IMPULSE_X));
            }
            break;
          case RIGHT_ARROW:
            if(!(body_get_velocity(player).x > MAX_VEL.x)){
              body_add_impulse(player, IMPULSE_X);
            }
            break;
          case ' ':
              if(colliding){
                printf("collision\n");
                if(!(body_get_velocity(player).y > MAX_VEL.y)){
                  body_add_impulse(player, IMPULSE_UP);
                }
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
  char* displayLife = (char *)malloc(sizeof(char)*100);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    if(step(scene, dt) == -1)
    {
      break;
    }
    sdl_clear();
    sprintf(displayScore, "Score: %zu", scene_get_score(scene));
    drawText(displayScore,27,(RGBColor){0,100,255}, (Vector){20,0});
    if(body_info_get_life(body_get_info(scene_get_body(scene, 0))) > 10) {
      sprintf(displayLife, "Lives: INVINCIBLE");
    }
    else{
      sprintf(displayLife, "Lives: %zu", body_info_get_life(body_get_info(scene_get_body(scene, 0))));
    }

    drawText(displayLife,27,(RGBColor){0,100,255}, (Vector){500,0});
    draw(scene, frame);
    frame++;
    sdl_show();
  }
  free(displayScore);
  free(displayLife);
  scene_free(scene);
  return 0;
}

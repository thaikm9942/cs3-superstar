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

typedef struct start {
  int ready;
} Start;
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
const RGBColor BLUE = (RGBColor){0.0, 0.0, 0.95};
const int NSTART_PLATFORMS = 6;
const int PLATFORM_DIST = 10;
#define M 6E26 // kg
#define g 9.8 // m / s^2
#define R (sqrt(G * M / g)) // m

const int IFRAMES = 100;

const int NORM_INV = 1000;
const int NORM_GROW = 3000;
const int NORM_GRAV = 7000;
const int NORM_BALL = 500;

const int HAZ_INV = 2000;
const int HAZ_GROW = 6000;
const int HAZ_GRAV = 700;
const int HAZ_BALL = 50;

const int BALL_INV = 4000;
const int BALL_GROW = 12000;
const int BALL_GRAV = 28000;
const int BALL_BALL = 10;

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
void add_point(Scene *scene, Body *platform) {
  Body* ball = scene_get_body(scene, 0);
  Body *point = point_init((Vector){body_get_centroid(platform).x, body_get_centroid(platform).y + 8},
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

Body *add_platform_altitude(Scene *scene, int y, bool trigger) {
  // Top of screen is Dimension.y, so make new platforms appear there.
  Body *platform = block_init((Vector){randomValue(0, BOUNDARY.x), y}, (Vector){30, 5}, PLATFORM_COLOR, 1, trigger);
  body_set_velocity(platform, DEFAULT_VEL);
  add_platform_physics(scene, platform);
  scene_add_body(scene, platform);
  return platform;
}

void add_platform_first(Scene *scene){
  Body *platform = block_init((Vector){BALL_POS.x, BALL_POS.y - 2 * BALL_RADIUS}, (Vector){30, 5}, PLATFORM_COLOR, 1, false);
  body_set_velocity(platform, DEFAULT_VEL);
  scene_add_body(scene, platform);
  add_platform_physics(scene, platform);
}

void add_fair_platforms(Scene *scene, double difficulty){
  bool trigger = true;
  for(double i = -BOUNDARY.y; i < BOUNDARY.y; i += PLATFORM_DIST + difficulty){
    Body *platform = add_platform_altitude(scene, i + 2 * BOUNDARY.y + (double) randomValue(-5, 5), trigger);
    trigger = false;
    if(rand() % 15 == 1){
      add_point(scene, platform);
    }
  }
}

void add_boundary(Scene *scene){
  Body *right = boundary_init((Vector){BOUNDARY.x + 15, 0}, (Vector){5, BOUNDARY.y * 2}, (RGBColor){0.0, 0.0, 0.0}, INFINITY);
  Body *left = boundary_init((Vector){-BOUNDARY.x - 15, 0}, (Vector){5, BOUNDARY.y * 2}, (RGBColor){0.0, 0.0, 0.0}, INFINITY);
  scene_add_body(scene, right);
  scene_add_body(scene, left);
}

void add_gravity_hazard(Scene *scene){
  gravity_hazard_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y + 15}, scene);
}

void add_ball_hazard(Scene *scene){
  moving_ball_hazard_init((Vector){BOUNDARY.x - 15, randomValue(0,BOUNDARY.y)}, (Vector) {-abs(randomValue(MAX_VEL.x, 10 * MAX_VEL.x)), DEFAULT_VEL.y}, abs(randomValue(1, 10)) * BALL_MASS, scene);
}

void add_power(Scene *scene, Body *power){
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
void add_star_invincibility(Scene *scene){
  Body *invincibility = invincibility_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y}, 4.0, 12.0, YELLOW);
  add_power(scene, invincibility);
}

void add_star_expand(Scene *scene){
  Body *expand = expand_init((Vector){randomValue(0, BOUNDARY.x), BOUNDARY.y}, 4.0, 12.0, BLUE);
  add_power(scene, expand);
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
  add_fair_platforms(scene, 0.0);
  //Experimental
  //add_gravity_hazard(scene);
  //add_ball_hazard(scene);
  return scene;
}

int next_platforms(Scene *scene){
  for(size_t i = 1; i < scene_bodies(scene); i++){
    if (body_info_get_type(body_get_info(scene_get_body(scene, i))) == PLATFORM_TRIGGER){
      return 0;
    }
  }
  return 1;
}

// Return 0 if game running, return -1 if game over
size_t step(Scene *scene, double dt, int last_score, Scene *background, int inv_rate, int grow_rate, int grav_rate, int ball_rate){
   if(rand() % inv_rate == 0){
     add_star_invincibility(scene);
   }

   if(rand() % grow_rate == 0){
     add_star_expand(scene);
   }
   if(rand() % grav_rate == 0){
     add_gravity_hazard(scene);
   }
   if(rand() % ball_rate == 0){
     add_ball_hazard(scene);
   }

  if(next_platforms(scene)){
    add_fair_platforms(scene, scene_get_score(scene) * 3.0);
  }
  Body* body = scene_get_body(scene, 0);
  BodyInfo* info = body_get_info(body);
  player_wrap(body, BOUNDARY);
  modulate_velocity(body);
  if(scene_get_status(scene)->isInvincible){
    body_set_color(body, YELLOW);
    if(!body_info_get_life_lock(info)){
      body_info_set_life(info, body_info_get_life(info) + 1);
    }
    body_info_set_life_lock(info, true);
  }
  else{
    body_set_color(body, RED);
    body_info_set_life_lock(info, false);
  }
  if(scene_get_status(scene)->isExpanded){
    body_star_set_radius_draw(body, BALL_RADIUS + 6, 5 + scene_get_score(scene));
  }
  else{
    body_star_set_radius_draw(body, BALL_RADIUS, 5 + scene_get_score(scene));
  }
  if(body_info_get_type(body_get_info(body)) != PLAYER){
    return -1;
  }
  scene_tick(scene, dt);
  scene_background_tick(background, dt, BOUNDARY);
  if(last_score != scene_get_score(scene))
  {
    body_star_set_num_sides(scene_get_body(scene, 0), (5 + scene_get_score(scene)));
  }
  return scene_get_score(scene);
}

// KeyHandler
void on_key(char key, KeyEventType type, void* aux_info) {
  Scene *scene = aux_info;
  Body* player = scene_get_body(scene, 0);
  Body* other = scene_get_body(scene, 1);
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
              for(int i = 1; i < scene_bodies(scene); i++){
                other =  scene_get_body(scene, i);
                if(find_collision(body_get_shape(player), body_get_shape(other)).collided){
                  if(!(body_get_velocity(player).y > MAX_VEL.y)){
                    body_add_impulse(player, IMPULSE_UP);
                  }
                  break;
                }
              }

      }
    }
    if(type == KEY_RELEASED)
    {
      Vector vec = body_get_velocity(player);
      vec.x = 0;
      body_set_velocity(player, vec);
    }
}

void start_key(char key, KeyEventType type, void* aux_info) {
  Start *s = aux_info;
  if(type == KEY_PRESSED && key == ' '){
    s->ready = 1;
  }
  else if(type == KEY_PRESSED && key == 'h'){
    s->ready = 2;
  }
  else if(type == KEY_PRESSED && key == 'b'){
    s->ready = 3;
  }
}

void init_background(Scene * background)
{
  RGBColor one = (RGBColor){0.576, 0.439, 0.858};
  RGBColor two = (RGBColor){0.729, 0.337, 0.827};
  RGBColor three = (RGBColor){1.0, 0.0, 1.0};
  RGBColor four = (RGBColor){0.854, 0.439, 0.839};
  RGBColor five = (RGBColor){0.933, 0.509, 0.933};
  RGBColor six = (RGBColor){0.827, 0.627, 0.827};
  List* shape1 = create_block((Vector){0, 25}, (Vector){400, 50});
  Body* block1 = body_init(shape1, 10, one, 10);
  body_set_velocity(block1, (Vector) {0, DEFAULT_VEL.y/2});
  List* shape2 = create_block((Vector){0, 75}, (Vector){400, 50});
  Body* block2 = body_init(shape2, 10, two, 10);
  body_set_velocity(block2, (Vector) {0, DEFAULT_VEL.y/2});
  List* shape3 = create_block((Vector){0, 125}, (Vector){400, 50});
  Body* block3 = body_init(shape3, 10, three, 10);
  body_set_velocity(block3, (Vector) {0, DEFAULT_VEL.y/2});
  List* shape4 = create_block((Vector){0, -125}, (Vector){400, 50});
  Body* block4 = body_init(shape4, 10, four, 10);
  body_set_velocity(block4, (Vector) {0, DEFAULT_VEL.y/2});
  List* shape5 = create_block((Vector){0, -75}, (Vector){400, 50});
  Body* block5 = body_init(shape5, 10, five, 10);
  body_set_velocity(block5, (Vector) {0, DEFAULT_VEL.y/2});
  List* shape6 = create_block((Vector){0, -25}, (Vector){400, 50});
  Body* block6 = body_init(shape6, 10, six, 10);
  body_set_velocity(block6, (Vector) {0, DEFAULT_VEL.y/2});
  scene_add_body(background, block1);
  scene_add_body(background, block2);
  scene_add_body(background, block3);
  scene_add_body(background, block4);
  scene_add_body(background, block5);
  scene_add_body(background, block6);
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
  srand(time(0));
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  sdl_clear();
  while(!sdl_is_done()){
    int frame = 0;
    Start *start = malloc(sizeof(start));
    start->ready = 0;
    sdl_on_key(start_key, start);
    drawText("SUPERSTAR!",72,(RGBColor){0,0,0}, (Vector){270,50});
    drawText("(Press space to begin)",72,(RGBColor){0,0,0}, (Vector){150,200});
    sdl_show();
    while(!sdl_is_done() && start->ready == 0){
      if(sdl_is_done()){
        free(start);
        return 0;
      }
    }
    sdl_clear();
    Scene * background = scene_init();
    Scene *scene = scene_init();
    init_background(background);
    init_scene(scene);
    sdl_on_key(on_key, scene);
    char* displayScore = (char *)malloc(sizeof(char)*100);
    char* displayLife = (char *)malloc(sizeof(char)*100);
    int last_life = 0;
    size_t last_score = 0;
      while(!sdl_is_done()){
        double dt = time_since_last_tick();
        last_life = body_info_get_life(body_get_info(scene_get_body(scene, 0)));
        if(start->ready == 1){
          last_score = step(scene, dt, last_score, background, NORM_INV, NORM_GROW, NORM_GRAV, NORM_BALL);
        }
        else if(start->ready == 2){
          last_score = step(scene, dt, last_score, background, HAZ_INV, HAZ_GROW, HAZ_GRAV, HAZ_BALL);
        }
        else if(start->ready == 3){
          last_score = step(scene, dt, last_score, background, BALL_INV, BALL_GROW, BALL_GRAV, BALL_BALL);
        }
        if(last_life > body_info_get_life(body_get_info(scene_get_body(scene, 0)))){
          activate_invincibility(scene_get_status(scene), IFRAMES);
        }
        if(last_score == -1)
        {
          break;
        }
        sdl_clear();

        draw(background, frame);
        sprintf(displayScore, "Score: %zu", scene_get_score(scene));
        drawText(displayScore,27,(RGBColor){0,100,255}, (Vector){20,0});
        sprintf(displayLife, "Lives: %zu", body_info_get_life(body_get_info(scene_get_body(scene, 0))));
        drawText(displayLife,27,(RGBColor){0,100,255}, (Vector){870,0});
        draw(scene, frame);

        frame++;
        sdl_show();
        if(sdl_is_done()){
          free(displayScore);
          free(displayLife);
          scene_free(scene);
          scene_free(background);
          return 0;
        }
        }

    free(start);
    free(displayScore);
    free(displayLife);
    scene_free(scene);
    scene_free(background);
    if(sdl_is_done()){
      return 0;
    }
    sdl_clear();
  }
  return 0;
}

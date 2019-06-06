#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "hazard.h"

// Constants:
const Vector DEFAULT_HAZARD_VEL = (Vector){0, -10};
const double HAZARD_RADIUS = 10;
const double HAZARD_MASS = 100;
// GRAV_COLOR and SPIKE_COLOR are both BLACK
const RGBColor GRAV_COLOR = (RGBColor){0.0, 0.0, 0.0};
const RGBColor SPIKE_COLOR = (RGBColor){0.0, 0.0, 0.0};
#define G 6.67E-11 // N m^2 / kg^2
#define G2 6.67E-3 // N m^2 / kg^2

void spike_hazard_init(Vector position, Scene* scene) {
  Body* spike = spike_init(position, HAZARD_RADIUS, INFINITY, SPIKE_COLOR, INFINITY);
  scene_add_body(scene, spike);
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body* body = scene_get_body(scene, i);
    BodyInfo* info = body_get_info(body);
    BodyType type = body_info_get_type(info);
    if(type == PLAYER){
      create_partial_collision_with_life(scene, 1, spike, body);
    }
    if(type == PLATFORM || type == PLATFORM_TRIGGER){
      create_partial_destructive_collision_with_life(scene, spike, body);
    }
  }
}

void gravity_hazard_init(Vector position, Scene* scene){
    Body* grav_body = gravity_ball_init(position, 0.5 * HAZARD_RADIUS, HAZARD_MASS, GRAV_COLOR, 1);
    body_set_velocity(grav_body, DEFAULT_HAZARD_VEL);
    scene_add_body(scene, grav_body);
    for(size_t i = 0; i < scene_bodies(scene); i++){
      Body* body = scene_get_body(scene, i);
      BodyInfo* info = body_get_info(body);
      BodyType type = body_info_get_type(info);
      if(type == PLAYER){
        create_newtonian_gravity(scene, G, grav_body, body);
        create_player_gravity_collision(scene, 1, body, grav_body);
      }
      if(type == SPIKE){
        create_partial_destructive_collision_with_life(scene, body, grav_body);
      }
    }
}

/*
void moving_ball_hazard_init(Vector position, Vector velocity, double mass, Scene* scene){
    Body* moving_ball_body = moving_ball_init(position, HAZARD_RADIUS, mass, BALL_COLOR, 1);
    scene_add_body(moving_ball_body, scene);
}
*/

/*
void spring_platforms_hazard_init(Vector position1, Vector position2, Vector dimension, double k, Scene* scene){
    Body* spring_platform_body1 = platform_spring_init(position1, dimension);
    Body* spring_platform_body2 = platform_spring_init(position2, dimension);
    scene_add_body(spring_platform_body1, scene);
    scene_add_body(spring_platform_body2, scene);
    create_spring_force(scene, k, spring_platform_body1, spring_platform_body2);
}
*/

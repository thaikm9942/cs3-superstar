#include "forces_game.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G_CONSTANT 9.8 // N m^2 / kg^2

void calculate_g_collision(ForceData *data){
  Body *player = data->body1;
  double g = data->force_constant;
  BodyInfo* player_info = body_get_info(player);
  if(!body_info_get_collision(player_info)){
    Vector force = (Vector){0, -g};
    body_add_force(player, force);
  }
}

void create_gravity(Scene *scene, Body *player){
  ForceData *data = force_data_init(G_CONSTANT, player, NULL);
  List *bodies_affected = list_init(1, NULL);
  list_add(bodies_affected, player);
  scene_add_bodies_force_creator(scene, (ForceCreator) calculate_g_collision, data, bodies_affected, free);
}

//NOTE: body2 is the Body being taken into consideration for lives
void repel_body_with_life(Body* body1, Body* body2, Vector axis, void* aux){
    PartialData* partial_data = (PartialData*) aux;
    double elasticity = partial_data->elasticity;
    bool partial = partial_data->partial;
    BodyInfo *info = body_get_info(body2);
    double reduced_mass;
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    double u_a = vec_dot(body_get_velocity(body1), axis);
    double u_b = vec_dot(body_get_velocity(body2), axis);
    if(m1 == INFINITY){
      reduced_mass = m2;
    }
    else if(m2 == INFINITY){
      reduced_mass = m1;
    }
    else {
      reduced_mass = (m1 * m2) / (m1 + m2);
    }
    double j_n = reduced_mass * (1 + elasticity) * (u_b - u_a);
    Vector impulse = vec_multiply(j_n, axis);
    body_add_impulse(body1, impulse);
    if(partial){
      if(body_info_get_life(info) == 0) {
        body_remove(body2);
      }
      else {
        body_info_set_life(info, body_info_get_life(info) - 1);
        body_add_impulse(body2, vec_negate(impulse));
      }
    }
    else{
      body_add_impulse(body2, vec_negate(impulse));
    }
}

// NOTE: body2 is the Body being taken into consideration for lives
// This accounts for partial destructive collision as well, in which if the
// collision is partial, then check if the body2 has 0 lives left, then remove
void destroy_body_with_life(Body* body1, Body* body2, Vector axis, void* aux){
  PartialData *partial_data = (PartialData*) aux;
  bool partial = partial_data->partial;
  BodyInfo* info = body_get_info(body2);
  if(partial){
    if(body_info_get_life(info) == 0) {
      body_remove(body2);
    }
    else {
      body_info_set_life(info, body_info_get_life(info) - 1);
    }
  }
  else{
    body_remove(body1);
    body_remove(body2);
  }
}

void calculate_special_collision(CollisionData* data){
  Body *player = data->body1;
  Body *platform = data->body2;
  CollisionInfo info = find_collision(body_get_shape(player), body_get_shape(platform));
  if(info.collided && !data->colliding){
    data->collision_handler(player, platform, info.axis, data->aux);
    data->colliding = true;
    body_info_set_collision((BodyInfo*)body_get_info(player), true);
  }
  if(!info.collided){
    data->colliding = false;
    body_info_set_collision((BodyInfo*)body_get_info(player), false);
  }
}

void create_special_collision(Scene *scene, Body *player, Body *platform,
CollisionHandler handler, void *aux, FreeFunc freer){
  CollisionData *data = collision_data_init(handler, aux, false, freer, player, platform);
  List *bodies_affected = list_init(2, NULL);
  list_add(bodies_affected, player);
  list_add(bodies_affected, platform);
  scene_add_bodies_force_creator(scene, (ForceCreator) calculate_special_collision, data, bodies_affected, (FreeFunc) collision_data_free);
}

/* All Superstar game collisions will be implemented here*/
//Target is the one being removed
void create_partial_collision_with_life(Scene *scene, double elasticity, Body *body, Body *target){
  PartialData *data = partial_data_init(elasticity, true);
  create_collision(scene, body, target, (CollisionHandler) repel_body_with_life, (void*) data, free);
}

void attach_body(Body* player, Body* platform, Vector axis, void* aux) {
  Vector player_vel = body_get_velocity(player);
  List* player_shape = body_get_shape(player);
  List* player_platform = body_get_shape(platform);
  if(player_vel.y < 0 && polygon_centroid(player_shape).y > polygon_centroid(player_platform).y){
    body_set_velocity(player, body_get_velocity(platform));
  }
}

void create_player_platform_collision(Scene *scene, Body* player, Body* platform){
  create_special_collision(scene, player, platform, (CollisionHandler) attach_body, NULL, NULL);
}

void create_partial_destructive_collision_with_life(Scene *scene, Body *object, Body *target){
  PartialData *data = partial_data_init(0.0, true);
  create_collision(scene, object, target, (CollisionHandler) destroy_body_with_life, (void*) data, free);
}

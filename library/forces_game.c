#include "forces_game.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G_CONSTANT 9.8E3 // N m^2 / kg^2
const double MIN_COLLISION_DISTANCE = 10;
void calculate_g_collision(ForceData *data){
  Body *player = data->body1;
  double g = data->force_constant;
  BodyInfo* player_info = body_get_info(player);
  if(!body_info_get_collision(player_info)){
    Vector force = (Vector){0, g};
    body_add_force(player, vec_negate(force));
  }
}

void repel_player(Body* body1, Body* body2, Vector axis, void* aux){
    PartialData *partial_data = (PartialData*) aux;
    double elasticity = partial_data->elasticity;
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
    printf("Adding impulse RP x: %f y: %f \n", impulse.x, impulse.y);
    assert(!isnan(impulse.x) && !isnan(impulse.y));
    body_add_impulse(body1, impulse);
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
    printf("RM: %f\n", reduced_mass);
    printf("UA: %f %f\n", u_a, u_b);
    double j_n = reduced_mass * (1 + elasticity) * (u_b - u_a);
    Vector impulse = vec_multiply(j_n, axis);
    assert(j_n != INFINITY && j_n != -INFINITY);
    printf("Adding impulse RPWL x: %f y: %f \n", impulse.x, impulse.y);
    assert(!isnan(impulse.x) && !isnan(impulse.y));
    body_add_impulse(body1, impulse);
    if(partial){
      if(body_info_get_life(info) == 0) {
        body_remove(body2);
      }
      else {
        printf("Life lost\n");
        printf("%d %d\n", body_info_get_type(body_get_info(body1)), body_info_get_type(body_get_info(body2)));
        printf("%f %f, %f %f", body_get_centroid(body1).x, body_get_centroid(body1).y, body_get_centroid(body2).x, body_get_centroid(body2).y);
        body_info_set_life(info, body_info_get_life(info) - 1);
        assert(!isnan(impulse.x) && !isnan(impulse.y));
        body_add_impulse(body2, vec_negate(impulse));
      }
    }
    else{
      assert(!isnan(impulse.x) && !isnan(impulse.y));
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
  BodyInfo* player_info = body_get_info(player);
  BodyInfo* platform_info = body_get_info(platform);
  double distance = (body_get_centroid(player).y + 5 - body_get_radius(player)) -
  (body_get_centroid(platform).y + body_get_radius(platform));
  CollisionInfo info = find_collision(body_get_shape(player), body_get_shape(platform));
  if(info.collided && !body_info_get_collision(player_info) && !body_info_get_collision(platform_info) && fabs(distance) < MIN_COLLISION_DISTANCE){
    body_info_set_collision(player_info, true);
    body_info_set_collision(platform_info, true);
    data->collision_handler(player, platform, info.axis, data->aux);
  }
  else if(body_info_get_collision(player_info) && body_info_get_collision(platform_info)){
    body_info_set_collision(player_info, false);
    body_info_set_collision(platform_info, false);
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

// Sets velocity of the player to the platform when it is slightly above the platform
// and is colliding
void attach_body(Body* player, Body* platform, Vector axis, void* aux) {
  Vector player_vel = body_get_velocity(player);
  double distance = (body_get_centroid(player).y + 2 - body_get_radius(player)) -
  (body_get_centroid(platform).y + body_get_radius(platform));
  if(player_vel.y < 0 && fabs(distance) < MIN_COLLISION_DISTANCE){
    body_set_velocity(player, (Vector){player_vel.x, body_get_velocity(platform).y});
    //Code below locks your Y-position immediately above the platform
    body_set_centroid(player, (Vector){body_get_centroid(player).x, body_get_centroid(platform).y + 10});
  }
}

// Modulates the player's velocity to make sure it never gets too high
void modulate_velocity(Body* player){
  Vector player_vel = body_get_velocity(player);
  if(player_vel.x > 100){
    body_set_velocity(player, (Vector){100, player_vel.y});
  }
  if(player_vel.x < -100){
    body_set_velocity(player, (Vector){-100, player_vel.y});
  }
  if(player_vel.y > 100){
    body_set_velocity(player, (Vector){player_vel.x, 100});
  }
  if(player_vel.y < -100){
    body_set_velocity(player, (Vector){player_vel.x, -100});
  }
}

// Creates player-platform collision, uses create_special_collision
void create_player_platform_collision(Scene *scene, Body* player, Body* platform){
  create_special_collision(scene, player, platform, (CollisionHandler) attach_body, NULL, NULL);
}

// In charge of handling player-point collisions, increment 1 to score in scene
// when this happens
void eat_point(Body* player, Body* point, Vector axis, void* aux){
  Scene* scene = (Scene*) aux;
  body_remove(point);
  scene_set_score(scene, scene_get_score(scene) + 1);
}

// Creates player-point collision
void create_player_point_collision(Scene *scene, Body* player, Body* point){
  create_collision(scene, player, point, (CollisionHandler) eat_point, (void*) scene, NULL);
}

// Creates player-gravity ball collision
void create_player_gravity_collision(Scene *scene, double elasticity, Body* player, Body* grav_ball){
  PartialData *partial = partial_data_init(elasticity, false);
  create_collision(scene, player, grav_ball, (CollisionHandler) repel_player, (void*) partial, free);
}
// Creates partial destructive collision
void create_partial_destructive_collision_with_life(Scene *scene, Body *object, Body *target){
  PartialData *data = partial_data_init(0.0, true);
  create_collision(scene, object, target, (CollisionHandler) destroy_body_with_life, (void*) data, free);
}

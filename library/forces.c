#include "forces.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct partial_data {
  double elasticity;
  bool partial;
  size_t* life;
};

struct force_data {
  double force_constant;
  Body *body1;
  Body *body2;
};

struct collision_data {
  //Checks to see if collision has occurred between two bodies before
  bool colliding;
  CollisionHandler collision_handler;
  void *aux;
  FreeFunc freer;
  Body *body1;
  Body *body2;
};

const double MIN_DISTANCE = 10;

/**
 * Adds a Newtonian gravitational force between two bodies in a scene.
 * See https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */

 PartialData *partial_data_init(double elasticity, bool partial, size_t* life){
   PartialData *partial_data = malloc(sizeof(PartialData));
   assert(partial_data != NULL);
   partial_data->elasticity = elasticity;
   partial_data->partial = partial;
   partial_data->life = life;
   return partial_data;
 }

 void partial_data_free(PartialData* data){
   if(data->life != NULL){
      free(data->life);
   }
   free(data);
 }

ForceData *force_data_init(double force_constant, Body *body1, Body *body2){
  ForceData *force_data = malloc(sizeof(ForceData));
  assert(force_data != NULL);
  force_data->force_constant = force_constant;
  force_data->body1 = body1;
  force_data->body2 = body2;
  return force_data;
}

CollisionData *collision_data_init(CollisionHandler handler, void* aux,
  bool colliding, FreeFunc freer, Body *body1, Body *body2){
  CollisionData *collision_data = malloc(sizeof(CollisionData));
  assert(collision_data != NULL);
  collision_data->colliding = colliding;
  collision_data->collision_handler = handler;
  collision_data->aux = aux;
  collision_data->freer = freer;
  collision_data->body1 = body1;
  collision_data->body2 = body2;
  return collision_data;
}

void force_data_free(ForceData* data){
  free(data);
}

void collision_data_free(CollisionData* data){
  if(data->freer != NULL){
    data->freer(data->aux);
  }
  free(data);
}

// A ForceCreator that calculates the gravitational force between 2 bodies
void calculate_g(ForceData *data){
  Body *body1 = data->body1;
  Body *body2 = data->body2;
  double g = data->force_constant;
  Vector dist_vec = vec_subtract(body_get_centroid(body1), body_get_centroid(body2));
  double dist = vec_magnitude(dist_vec);
  Vector unit_vec = vec_multiply(1.0 / dist, dist_vec);
  double m1 = body_get_mass(body1);
  double m2 = body_get_mass(body2);
  double force_mag = g * m1 * m2 / pow(dist, 2.0);
  // Force from body 1 to body 2
  Vector force = vec_multiply(force_mag, unit_vec);
  if(dist > MIN_DISTANCE){
    body_add_force(body1, vec_negate(force));
    body_add_force(body2, force);
  }
}

// A ForceCreator that calculates the spring force using Hooke's Law between
// 2 bodies
void calculate_k(ForceData *data){
  Body *body1 = data->body1;
  Body *body2 = data->body2;
  double k = data->force_constant;
  Vector dist_vec = vec_subtract(body_get_centroid(body1), body_get_centroid(body2));
  double dist = vec_magnitude(dist_vec);
  Vector unit_vec = vec_multiply(1.0 / dist, dist_vec);
  double force_mag = k * dist;
  // Force from body 1 to body 2
  Vector force = vec_multiply(force_mag, unit_vec);
  body_add_force(body1, vec_negate(force));
  body_add_force(body2, force);
}

// A ForceFunction that calculates the drag froce on a body using F= -gamma * v
void calculate_gamma(ForceData *data){
  Body *body = data->body1;
  double gamma = data->force_constant;
  // Force from body 1 to body 2
  Vector force = vec_negate(vec_multiply(gamma, body_get_velocity(body)));
  body_add_force(body, force);
}

void create_newtonian_gravity(Scene *scene, double G, Body *body1, Body *body2){
  ForceData *data = force_data_init(G, body1, body2);
  List *bodies_affected = list_init(2, NULL);
  list_add(bodies_affected, body1);
  list_add(bodies_affected, body2);
  scene_add_bodies_force_creator(scene, (ForceCreator) calculate_g, data, bodies_affected, (FreeFunc) force_data_free);
}

void create_spring(Scene *scene, double k, Body *body1, Body *body2){
  ForceData *data = force_data_init(k, body1, body2);
  List *bodies_affected = list_init(2, NULL);
  list_add(bodies_affected, body1);
  list_add(bodies_affected, body2);
  scene_add_bodies_force_creator(scene, (ForceCreator) calculate_k, data, bodies_affected, (FreeFunc) force_data_free);
}

void create_drag(Scene *scene, double gamma, Body *body) {
  ForceData *data = force_data_init(gamma, body, NULL);
  List *bodies_affected = list_init(1, NULL);
  list_add(bodies_affected, body);
  scene_add_bodies_force_creator(scene, (ForceCreator) calculate_gamma, data, bodies_affected, (FreeFunc) force_data_free);
}

//Collision handlers
void repel_body(Body* body1, Body* body2, Vector axis, void* aux){
    PartialData *partial_data = (PartialData*) aux;
    double elasticity = partial_data->elasticity;
    bool partial = partial_data->partial;
    size_t *life = partial_data->life;
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
    if(!partial || life == NULL){
      body_add_impulse(body2, vec_negate(impulse));

    }
    else if(partial && *life > 0){
      *life = *life - 1;
      body_add_impulse(body2, vec_negate(impulse));
    }
    else if(partial && *life == 0){
      body_remove(body2);
    }
}


/* This method is now obsolete.
void destroy_body(Body* body1, Body* body2, Vector axis, void* aux){
  body_remove(body1);
  body_remove(body2);
}
*/

// This accounts for partial destructive collision as well, in which if the
// collision is partial, then only the 2nd body is destroyed
void destroy_body(Body* body1, Body* body2, Vector axis, void* aux){
  if(aux != NULL){
    PartialData *partial_data = (PartialData*) aux;
    bool partial = partial_data->partial;
    if(!partial){
        body_remove(body1);
    }
  }
  else {
    body_remove(body1);
  }
  body_remove(body2);
}

void calculate_collision(CollisionData* data){
  Body *body1 = data->body1;
  Body *body2 = data->body2;
  CollisionInfo info = find_collision(body_get_shape(body1), body_get_shape(body2));
  if(info.collided && !data->colliding){
    data->collision_handler(body1, body2, info.axis, data->aux);
    data->colliding = true;
  }
  if(!info.collided){
    data->colliding = false;
  }
}

void create_collision(Scene *scene, Body *body1, Body *body2,
CollisionHandler handler, void *aux, FreeFunc freer){
  CollisionData *data = collision_data_init(handler, aux, false, freer, body1, body2);
  List *bodies_affected = list_init(2, NULL);
  list_add(bodies_affected, body1);
  list_add(bodies_affected, body2);
  scene_add_bodies_force_creator(scene, (ForceCreator) calculate_collision, data, bodies_affected, (FreeFunc) collision_data_free);
}

void create_destructive_collision(Scene *scene, Body *body1, Body *body2) {
  create_collision(scene, body1, body2, (CollisionHandler) destroy_body, NULL, NULL);
}

void create_partial_collision(Scene *scene, double elasticity, Body *body, Body *target){
  PartialData *partial = partial_data_init(elasticity, true, NULL);
  create_collision(scene, body, target, (CollisionHandler) repel_body, (void*) partial, (FreeFunc) partial_data_free);
}

//Target is the one being removed
void create_partial_collision_with_life(Scene *scene, double elasticity, Body *body, Body *target){
  BodyInfo* info = body_get_info(target);
  PartialData *partial = partial_data_init(elasticity, true, body_info_get_life(info));
  create_collision(scene, body, target, (CollisionHandler) repel_body, (void*) partial, (FreeFunc) partial_data_free);
}

void create_physics_collision(Scene *scene, double elasticity, Body *body1, Body *body2){
  PartialData *partial = partial_data_init(elasticity, false, NULL);
  create_collision(scene, body1, body2, (CollisionHandler) repel_body, (void*) partial, (FreeFunc) partial_data_free);
}

/* All Superstar game collisions will be implemented here*/
void create_partial_destructive_collision(Scene *scene, Body *object, Body *target){
  PartialData *partial = partial_data_init(0.0, true, NULL);
  create_collision(scene, object, target, (CollisionHandler) destroy_body, (void*) partial, (FreeFunc) partial_data_free);
}

void attach_body(Body* player, Body* platform, Vector axis, void* aux) {
  Vector player_vel = body_get_velocity(player);
  if(player_vel.y < 0){
    body_set_velocity(player, body_get_velocity(platform));
  }
}

void create_player_platform_collision(Scene *scene, Body* player, Body* platform){
  create_collision(scene, player, platform, (CollisionHandler) attach_body, NULL, NULL);
}

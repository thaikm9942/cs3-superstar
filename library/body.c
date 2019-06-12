#include "body.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include <math.h>
#include "shape.h"

const int DEBUG = 0;
// 0 is false 1 is true. When true, all assert statements and print statements
// run. Used to handle the epic random crash problem.

Body *body_init(List *shape, double mass, RGBColor color, double radius){
    Body *thisBod = malloc(sizeof(Body));
    assert(thisBod != NULL);
    thisBod->points = shape;
    thisBod->m = mass;
    thisBod->c = color;
    thisBod->vel = VEC_ZERO;
    thisBod->theta = 0.0;
    thisBod->force = VEC_ZERO;
    thisBod->impulse = VEC_ZERO;
    thisBod->info = NULL;
    thisBod->info_freer = NULL;
    thisBod->removed = false;
    thisBod->radius = radius;
    return thisBod;
}

Body *body_init_with_info(
    List *shape, double mass, RGBColor color, void *info, FreeFunc info_freer, double radius){
    Body *thisBod = malloc(sizeof(Body));
    assert(thisBod != NULL);
    thisBod->points = shape;
    thisBod->m = mass;
    thisBod->c = color;
    thisBod->vel = VEC_ZERO;
    thisBod->theta = 0.0;
    thisBod->force = VEC_ZERO;
    thisBod->impulse = VEC_ZERO;
    thisBod->info = info;
    thisBod->info_freer = info_freer;
    thisBod->removed = false;
    thisBod->radius = radius;
    return thisBod;
}


void body_free(Body *body){
    if(body->info_freer != NULL){
      body->info_freer(body->info);
    }
    list_free(body->points);
    free(body);
}

List *body_get_shape(Body *body){
    return body->points;
}

Vector body_get_centroid(Body *body){
    return polygon_centroid(body->points);
}

Vector body_get_velocity(Body *body){
    return body->vel;
}

void body_set_color(Body *body, RGBColor color){
    body->c = color;
}

RGBColor body_get_color(Body *body){
    return body->c;
}

double body_get_mass(Body *body){
  return body->m;
}

void *body_get_info(Body *body){
  return body->info;
}

void body_remove(Body *body){
  if(!body->removed){
    body->removed = true;
  }
}

bool body_is_removed(Body *body){
  return body->removed;
}

/*Extra functionality*/
Vector body_get_force(Body *body){
    return body->force;
}

Vector body_get_impulse(Body *body){
    return body->impulse;
}

double body_get_radius(Body *body){
  return body->radius;
}

/*Set functions*/
void body_set_shape(Body *body, List* new_shape) {
  List* old = body->points;
  body->points = new_shape;
  list_free(old);
}

// Only used for objects that have a radius or a y-height
void body_set_radius(Body* body, double new_r){
  body->radius = new_r;
}

void body_set_centroid(Body *body, Vector x){
    polygon_translate(body->points, vec_negate(body_get_centroid(body)));
    polygon_translate(body->points, x);
}

void body_set_velocity(Body *body, Vector v){
    body->vel = v;
}

void body_set_rotation(Body *body, double angle){
    polygon_rotate(body->points, angle - body->theta, body_get_centroid(body));
    body->theta = angle;
}

void body_set_force(Body *body, Vector force){
    body->force = force;
}

void body_set_impulse(Body *body, Vector impulse){
    body->impulse = impulse;
}

void body_add_force(Body *body, Vector force){
  body_set_force(body, vec_add(body_get_force(body), force));
}

void body_add_impulse(Body *body, Vector impulse){
  body_set_impulse(body, vec_add(body_get_impulse(body), impulse));
}

void body_tick(Body *body, double dt){
  Vector vel_before = body_get_velocity(body);
  Vector total_impulse = vec_add(body_get_impulse(body), vec_multiply(dt, body_get_force(body)));
  if(DEBUG)
  {
    // Legacy of the random crashing bug. The Nan was traced using this code
    // and this code is left for use if the bug is not fully gone as thought.
    // Set the const to DEBUG to use.
    assert(!isnan(vel_before.y) && !isnan(vel_before.x));
    assert(!isnan(total_impulse.y) && (!isnan(total_impulse.x)));
    assert(!isnan(body_get_mass(body)));
    printf("Mass %f\n", body_get_mass(body));
    printf("Type %d\n", body_info_get_type(body_get_info(body)));
    float repMass = 1.0 / body_get_mass(body);
    // 1 over 0 is infinity and infinity * another number is nan
    printf("Reciprocal Mass%f\n", repMass);
    assert(!isnan(repMass));
    Vector addtions = vec_multiply(repMass, total_impulse);
    assert(!isnan(addtions.y) && !isnan(addtions.x));
    Vector new_vel = vec_add(vel_before, addtions);
    assert(!isnan(new_vel.y) && !isnan(new_vel.x));
  }
  if(body_get_mass(body) <= 0 && body_info_get_type(body_get_info(body)) == 6)
  {
    // THis is bad
    // Periodically, the mass of an object of type 6 become negative or zero
    // We don't know why, but if so the mass is reset to 200 (default value)
    if(DEBUG){
      printf("Mass fixed to be non negative");
    }
    body->m = 200;
  }


  body_set_velocity(body, vec_add(vel_before, vec_multiply(1.0 / body_get_mass(body), total_impulse)));
  Vector avg_vel = vec_multiply(1.0/2.0, vec_add(vel_before, body_get_velocity(body)));
  body_set_centroid(body, vec_add(body_get_centroid(body), vec_multiply(dt, avg_vel)));
  body_set_force(body, VEC_ZERO);
  body_set_impulse(body, VEC_ZERO);
}

/* All extra functionality */
void body_accelerate(Body * body, Vector a, double dt)
{
  body_set_velocity(body, vec_add(body_get_velocity(body), (vec_multiply(dt, a))));
}

void player_wrap(Body *body, Vector max)
{
  Vector centroid = body_get_centroid(body);
  if(centroid.x > max.x || centroid.x < -max.x){
    centroid.x = -centroid.x;
    body_set_centroid(body, centroid);
  }
  if(centroid.y > max.y){
    centroid.y = max.y;
    Vector v = body_get_velocity(body);
    v.y = 0;
    body_set_velocity(body, v);
    body_set_centroid(body, centroid);
  }
}

#include "body.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include <math.h>

Body *body_init(List *shape, double mass, RGBColor color){
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
    return thisBod;
}

Body *body_init_with_info(
    List *shape, double mass, RGBColor color, void *info, FreeFunc info_freer){
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

/*Set functions*/
void body_set_shape(Body *body, List* new_shape) {
  List* old = body->points;
  body->points = new_shape;
  list_free(old);
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

// Wraps bodies around
void alien_wrap(Body *body, Vector max)
{
  Vector centroid = body_get_centroid(body);
  if(centroid.x > max.x || centroid.x < -max.x){
    centroid.y -= 150;
    body_set_centroid(body, centroid);
    body_set_velocity(body, (Vector){-1 * body_get_velocity(body).x, body_get_velocity(body).y});
  }
}

void player_wrap(Body *body, Vector max)
{
  Vector centroid = body_get_centroid(body);
  if(centroid.x > max.x || centroid.x < -max.x){
    centroid.x = -centroid.x;
    body_set_centroid(body, centroid);
  }
}

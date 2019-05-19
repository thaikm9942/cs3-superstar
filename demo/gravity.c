#include "sdl_wrapper.h"
#include "polygon.h"
#include <vec_list.h>
#include <vector.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array_list.h"
#include "pointy_shape.h"

const Vector BOUNDARY = {
  .x = 500.0,
  .y = 250.0
};
const double GRAVITY = 980.0;
const double ROTATION = -M_PI * 2;
const double MAXTIME = 2.0;

//Determines whether a bounce occurs, and if so, which dimensions to bounce
Vector bounceVector(VectorList *obj, Vector displacement, Vector boundary){
  Vector bounce = vec_init(1.0, 1.0);
  for(size_t i = 0; i < vec_list_size(obj); i++){
    Vector point = vec_list_get(obj, i);
    if(fabs(point.y + displacement.y) > boundary.y){
      bounce.y = -1.0;
    }
  }
  return bounce;
}

int is_out(VectorList *obj){
  for(size_t i=0; i<vec_list_size(obj); i++){
    if(vec_list_get(obj, i).x < BOUNDARY.x && vec_list_get(obj, i).y < BOUNDARY.y){
      return 0;
    }
  }
  return 1;
}

void compute_new_positions(ArrayList *list, double dt){
  for(size_t i = 0; i < array_list_size(list); i++){
    VectorList *obj = get_points(array_list_get(list, i));
    Vector vel = get_velocity(array_list_get(list, i));
    double elasticity = get_elasticity(array_list_get(list, i));
    polygon_rotate(obj, ROTATION * dt, polygon_centroid(obj));
    Vector b = bounceVector(obj, vec_multiply(dt, vel), BOUNDARY);
    if(b.y != 1.0){
      vel.y *= b.y * elasticity;
    }
    polygon_translate(obj, vec_multiply(dt, vel));
    vel.y -= dt * GRAVITY;
    set_velocity(array_list_get(list, i), vel);
    if(is_out(obj)){
      free_shape(array_list_get(list, i));
      array_list_remove_at(list, i);
      i--;
    }
  }
  return;
}

int main(int argc, char *argv[]){
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  ArrayList *list = array_list_init(1);
  double temptime = MAXTIME;
  while(!sdl_is_done()){
    if(MAXTIME < temptime){
      array_list_add(list, pointy_shape_rand());
      VectorList *temp = get_points(array_list_get(list, array_list_size(list)-1));
      polygon_translate(temp, vec_init(-BOUNDARY.x + vec_list_get(temp, 0).y, BOUNDARY.y - vec_list_get(temp, 0).y));
      temptime = 0.0;
    }
    double dt = time_since_last_tick();
    temptime += dt;
    compute_new_positions(list, dt);
    sdl_clear();
    for(size_t i = 0; i < array_list_size(list); i++){
      PointyShape *curGon = array_list_get(list, i);
      sdl_draw_polygon(get_points(curGon), get_color(curGon).red, get_color(curGon).blue, get_color(curGon).green);
    }
    sdl_show();
  }
  array_list_free(list);
  return 0;
}

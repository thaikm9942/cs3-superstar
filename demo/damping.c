#include "sdl_wrapper.h"
#include "polygon.h"
#include "body.h"
#include "color.h"
#include "scene.h"
#include "list.h"
#include <vector.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "forces.h"

const Vector BOUNDARY = {
  .x = 500.0,
  .y = 250.0
};
const RGBColor WHITE = (RGBColor){1.0, 1.0, 1.0};
const double BALL_RADIUS = 10;
const double BALL_MASS = 10;
const double COLOR_FREQ = 0.5;
const double STIFFNESS = 50;
const double DRAG = 0.9;

Vector *init_vec(Vector v){
  Vector *vector = malloc(sizeof(v));
  vector->x = v.x;
  vector->y = v.y;
  return vector;
}

Body *init_ball(Vector position, double mass, RGBColor color){
  List *ball = list_init(75, free);
  for(double angle = 0.0; angle < 2 * M_PI; angle += 0.05){
    list_add(ball, init_vec(vec_multiply(BALL_RADIUS, (Vector){cos(angle), sin(angle)})));
  }
  polygon_translate(ball, position);
  return body_init(ball, mass, color);
}

RGBColor rainbow(double seed){
  seed *= COLOR_FREQ;
  return (RGBColor){(1 + sin(seed))/2.0, (1 + sin(seed + 2))/2.0, (1+sin(seed + 4))/2.0};
}

Scene *init_scene(void){
  Scene *scene = scene_init();
  double x = BALL_RADIUS;
  double y = BOUNDARY.y;
  double stiffness = STIFFNESS;
  for(double i = 0; i < BOUNDARY.x / BALL_RADIUS; i++){
    Body *anchorBall = init_ball((Vector){x - BOUNDARY.x, 0.0}, INFINITY, WHITE);
    Body *freeBall = init_ball((Vector){x - BOUNDARY.x, BOUNDARY.y - x / 10.0}, BALL_MASS, rainbow(i));
    create_spring(scene, stiffness, freeBall, anchorBall);
    create_drag(scene, DRAG, freeBall);
    scene_add_body(scene, anchorBall);
    scene_add_body(scene, freeBall);
    x += BALL_RADIUS * 2;
    y -= 2 * BOUNDARY.y / (BOUNDARY.x / BALL_RADIUS);
    stiffness *= 0.9;
  }
  return scene;
}

void compute_new_positions(Scene *scene, double dt){
  scene_tick(scene, dt);
}

int main(int argc, char *argv[]){
  sdl_init(vec_negate(BOUNDARY), BOUNDARY);
  Scene *scene = init_scene();
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    compute_new_positions(scene, dt);
    sdl_clear();
    for(size_t i = 0; i < scene_bodies(scene); i++){
      Body *body = scene_get_body(scene, i);
      List *polygon = body_get_shape(body);
      sdl_draw_polygon(polygon, body_get_color(body));
    }
    sdl_show();
  }
  scene_free(scene);
  return 0;
}

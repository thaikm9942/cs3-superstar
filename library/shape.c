#include "../include/shape.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct body_info{
  BodyType* type;
  size_t* life;
};

BodyInfo* body_info_init(BodyType* type, size_t* life){
  BodyInfo* info = malloc(sizeof(BodyInfo));
  assert(info != NULL);
  info->type = type;
  info->life = life;
  return info;
}

void body_info_free(BodyInfo* info){
  free(info->type);
  free(info->life);
  free(info);
}

BodyType* body_info_get_type(BodyInfo* info){
  return info->type;
}

size_t* body_info_get_life(BodyInfo* info){
  return info->life;
}

List *rotate_points(int sides, Vector point){
  double angle = 2 * M_PI / sides;
  List *rotated = list_init(sides, free);
  for(size_t i = 0; i < sides; i++) {
    list_add(rotated, vec_init(vec_rotate(point, angle * i)));
  }
  return rotated;
}

// Create a star shape that acts as the visual in counterclockwise
List *create_star(int sides, Vector position, double radius){
    double big_r = radius;
    double small_r = big_r / 2;
    Vector outer_point = vec_add(VEC_ZERO, (Vector){0, big_r});
    Vector inner_point = vec_add(VEC_ZERO, (Vector){small_r * cos(M_PI/2 + M_PI/sides), small_r * sin(M_PI/2 + M_PI/sides)});

    List *outer = rotate_points(sides, outer_point);
    List *inner = rotate_points(sides, inner_point);
    List *star = list_init(2 * sides, free);

    // Combines the list of outer and inner points in counterclockwise direction
    for(size_t i = 0; i < sides; i++){
      list_add(star, vec_init(*(Vector*)list_get(outer, i)));
      list_add(star, vec_init(*(Vector*)list_get(inner, (i % sides))));
    }

    list_free(outer);
    list_free(inner);

    polygon_translate(star, position);
    return star;
}

// Creates a rectangular shaped block that acts as the visual in counterclockwise
// order
List *create_block(Vector position, Vector dimension){
  List *block = list_init(4, free);
  list_add(block, vec_init((Vector){dimension.x / 2.0, dimension.y / 2.0}));
  list_add(block, vec_init((Vector){dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, -dimension.y / 2.0}));
  list_add(block, vec_init((Vector){-dimension.x / 2.0, dimension.y / 2.0}));
  polygon_translate(block, position);
  return block;
}

// Creates a circle shape that acts as the visual in counterclockwise
// order
List *create_ball(Vector position, double radius){
  List *ball = list_init(75, free);
  for(double angle = 0.0; angle < 2 * M_PI; angle += 0.05){
    list_add(ball, vec_init(vec_multiply(radius, (Vector){cos(angle), sin(angle)})));
  }
  polygon_translate(ball, position);
  return ball;
}

// Initializes a star Body using a position, dimension, mass and color with a specified
// info of PLAYER
Body *star_init(int sides, Vector position, double radius, double mass, RGBColor color, int life){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = PLAYER;
  int* body_life = malloc(sizeof(int));
  *body_life = life;
  BodyInfo* body_info = body_info_init(type, body_life);
  return body_init_with_info(create_star(sides, position, radius), mass, color, (void*) body_info, (FreeFunc) body_info_free);
}

// Initializes a block Body using a position, dimension and color with a specified
// info of PLATFORM
Body *block_init(Vector position, Vector dimension, RGBColor color, int life){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = PLATFORM;
  int* body_life = malloc(sizeof(int));
  *body_life = life;
  BodyInfo* body_info = body_info_init(type, body_life);
  return body_init_with_info(create_block(position, dimension), INFINITY, color, (void*) body_info, (FreeFunc) body_info_free);
}

Body *ball_init(Vector position, double radius, double mass, RGBColor color, int life, BodyType* type){
  int* body_life = malloc(sizeof(size_t));
  *body_life = life;
  BodyInfo* body_info = body_info_init(type, body_life);
  return body_init_with_info(create_ball(position, radius), mass, color, (void*) body_info, (FreeFunc) body_info_free);
}

// Initializes a block Body using a position, dimension, mass and color with a specified
// info of POINT
Body *point_init(Vector position, double radius, double mass, RGBColor color, int life){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = POINT;
  return ball_init(position, radius, mass, color, life, type);
}

Body *gravity_ball_init(Vector position, double radius, double mass, RGBColor color, int life){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = GRAVITY_BALL;
  return ball_init(position, radius, mass, color, life, type);
}

Body *moving_ball_init(Vector position, double radius, double mass, RGBColor color, int life){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = MOVING_BALL;
  return ball_init(position, radius, mass, color, life, type);
}

Body *spike_init(Vector position, double radius, double mass, RGBColor color, int life){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = SPIKE;
  int* body_life = malloc(sizeof(int));
  *body_life = life;
  BodyInfo* body_info = body_info_init(type, body_life);
  return body_init_with_info(create_star(3, position, radius), mass, color, (void*) body_info, (FreeFunc) body_info_free);
}

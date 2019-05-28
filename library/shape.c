#include "shape.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

typedef enum {
    PLATFORM,
    PLAYER,
    SPIKES,
} BodyType;

// Will later add area-density to the platform
struct shape {
  Body* body;
  int life;
};

List *rotate_points(int sides, Vector point){
  double angle = 2 * M_PI / sides;
  List *rotated = list_init(sides, free);
  for(size_t i = 0; i < sides; i++) {
    list_add(rotated, init_vec(vec_rotate(point, angle * i)));
  }
  return rotated;
}

// Create a star shape that acts as the visual in counterclockwise
List *create_star(int sides, Vector position, Vector radius){
    double big_r = radius;
    double small_r = big_r / 2;
    Vector outer_point = vec_add(VEC_ZERO, (Vector){0, big_r});
    Vector inner_point = vec_add(VEC_ZERO, (Vector){small_r * cos(M_PI/2 + M_PI/sides), small_r * sin(M_PI/2 + M_PI/sides)});

    List *outer = rotate_points(sides, outer_point);
    List *inner = rotate_points(sides, inner_point);
    List *star = list_init(2 * sides, free);

    // Combines the list of outer and inner points in counterclockwise direction
    for(size_t i = 0; i < sides; i++){
      list_add(star, init_vec(*(Vector*)list_get(outer, i)));
      list_add(star, init_vec(*(Vector*)list_get(inner, (i % sides))));
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

Body *star_init(int sides, Vector position, Vector radius, double mass, RGBColor color){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = PLAYER;
  return body_init_with_info(create_star(sides, position, radius), mass, color, type, free);
}
// Initializes a block Body using a position, dimension and color with a specified
// info of PLATFORM
Body *block_init(Vector position, Vector dimension, RGBColor color){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = PLATFORM;
  return body_init_with_info(create_block(position, dimension), INFINITY, color, type, free);
}

Shape* platform_init(Vector position, Vector dimension, RGBColor color, int life){
  Shape* platform = malloc(sizeof(Shape));
  assert(platform != NULL);
  Body* block = block_init(position, dimension, color);
  platform->body = block;
  platform->life = life;
  return platform;
}

void shape_free(Shape* shape){
  body_free(shape->body);
  free(shape);
}

Body* shape_get_body(Shape* shape){
  return shape->body;
}

int shape_get_life(Shape* shape){
  return shape->life;
}

void draw_shape(Shape* shape){
  Body* body = shape_get_body(shape);
  List* polygon = body_get_shape(body);
  sdl_draw_polygon(polygon, body_get_color(body));
}

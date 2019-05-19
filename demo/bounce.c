#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"
#include "polygon.h"

// VEC_MAX must correspond to the 2:1 ratio outlined in sdl_wrapper.c
const Vector VEC_MAX = (Vector){200, 100};
const double SPEED = 200;
const double ANGLE = M_PI / 4;

const int SIDES = 7;
const double RADIUS = 10;
/**
 * Provides a list of points equally distributed around a circle by an angle
 * determine by the number of sides
 * @param point a given starting point
 * @param sides the number of sides
 * @return a list of points
 */
VectorList *rotate_points(Vector point, size_t sides){
  double angle = 2 * M_PI / sides;
  VectorList *rotated = vec_list_init(sides);
  for(size_t i = 0; i < sides; i++) {
    vec_list_add(rotated, vec_rotate(point, angle * i));
  }
  return rotated;
}

/**
 * Returns a list of points need to draw a star in counterclockwise order
 * starting from the topmost point in the outer circle and the bottom point
 * in the inner circle before translating them to the desired center
 * @param center the point at which the center of the star is located
 * @param sides the number of sides
 * @param big_r the radius between the center and the outer points
 * @return a list of points needed to make the star
 */
VectorList *make_star(Vector center, size_t sides, double big_r) {
  // Takes a point as the top vertex of the star and compute the rest of
  // its vertices by rotation it 2 * M_PI / sides
  double small_r = big_r / 2;
  Vector outer_point = vec_add(VEC_ZERO, (Vector){0, big_r});
  Vector inner_point = vec_add(VEC_ZERO, (Vector){0, -small_r});


  VectorList *outer = rotate_points(outer_point, sides);
  VectorList *inner = rotate_points(inner_point, sides);

  VectorList *star = vec_list_init(sides * 2);

  // Combines the list of outer and inner points in counterclockwise direction
  for(size_t i = 0; i < sides; i++){
    vec_list_add(star, outer->data[i]);
    vec_list_add(star, inner->data[(i + sides / 2 + 1) % sides]);
  }

  polygon_translate(star, center);

  vec_list_free(inner);
  vec_list_free(outer);
  return star;
}

/**
 * Returns a list of points in a star scaled by a factor scalar
 * @param scalar scaling factor
 * @return a list of scaled points in a star
 */
VectorList *scale_star(VectorList* star, int scalar) {
  for(size_t i = 0; i < star->size; i++) {
    vec_list_set(star, i, vec_multiply(scalar, star->data[i]));
  }
  return star;
}

/**
 * Calculates the actual displacement taking into account the collisions and
 * reflections in both x and y direction
 * @param c the current centroid of the polygon
 * @param d the desired displacement
 * @param max the maximum point in a direction
 * @return the actual displacement
 */
double displacement(double c, double d, double max) {
  if(c + d + RADIUS > max) {
    return 2 * max - 2 * RADIUS - 2 * c - d;
  }
  else if(c + d - RADIUS < 0) {
    return 2 * RADIUS - 2 * c - d;
  }
  else {
    return d;
  }
}

/**
 * Computes new positions of a star after certain time dt with a set velocity
 * and rotation
 * @param positions the current points in a star
 * @param dt change in time
 * @param max the maximum point in a direction
 * @return a list of the new points in the star after movement
 */
VectorList *compute_new_pos(VectorList *positions, double dt, Vector *velocity) {
  Vector center = polygon_centroid(positions);

  double dx = displacement(center.x, velocity->x * dt, VEC_MAX.x);
  if(dx != (velocity->x * dt)){
    velocity->x = -velocity->x;
  }
  double dy = displacement(center.y, velocity->y * dt, VEC_MAX.y);
  if(dy != (velocity->y * dt)){
    velocity->y = -velocity->y;
  }

  polygon_translate(positions, (Vector){dx, dy});
  polygon_rotate(positions, 2 * M_PI / 1080, center);

  return positions;
}

int main(int argc, char *argv[]) {
   Vector dir = (Vector){cos(ANGLE), sin(ANGLE)};
   Vector center = vec_multiply(0.5, vec_add(VEC_ZERO, VEC_MAX));
   Vector* velocity = malloc(sizeof(Vector));
   velocity->x = SPEED * dir.x;
   velocity->y = SPEED * dir.y;
   VectorList *star = make_star(center, SIDES, RADIUS);

   sdl_init(VEC_ZERO, VEC_MAX);
   while(!sdl_is_done()) {
     double dt = time_since_last_tick();
     star = compute_new_pos(star, dt, velocity);
     sdl_clear();
     sdl_draw_polygon(star, 0, 1, 0);
     sdl_show();
   }
   free(velocity);
   vec_list_free(star);
}

#include "../include/vector.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Declaring the constant zero vector, i.e. (0, 0).
const Vector VEC_ZERO = {
  .x = 0.0,
  .y = 0.0
};

Vector *vec_init(Vector v){
  Vector *vector = malloc(sizeof(v));
  vector->x = v.x;
  vector->y = v.y;
  return vector;
}

Vector vec_add(Vector v1, Vector v2) {
  Vector sum = {
    .x = v1.x + v2.x,
    .y = v1.y + v2.y
  };
  return sum;
}

Vector vec_subtract(Vector v1, Vector v2) {
  return vec_add(v1, vec_negate(v2));
}

Vector vec_negate(Vector v) {
  return vec_multiply(-1, v);
}

Vector vec_multiply(double scalar, Vector v) {
  Vector product = {
    .x = v.x * scalar,
    .y = v.y * scalar
  };
  return product;
}

double vec_dot(Vector v1, Vector v2) {
  double dot_product = v1.x * v2.x + v1.y * v2.y;
  return dot_product;
}

double vec_cross(Vector v1, Vector v2) {
  double cross_product = v1.x * v2.y - v2.x * v1.y;
  return cross_product;
}

double vec_get_x(Vector v)
{
  return v.x;
}

double vec_get_y(Vector v)
{
  return v.y;
}

double vec_magnitude(Vector v){
  return sqrt(vec_dot(v, v));
}

Vector vec_rotate(Vector v, double angle) {
/** Standard rotation matrix has form (cos(theta), -sin(theta); sin(theta),
 * cos(theta);
 */
 Vector new_v = {
   .x = v.x * cos(angle) - v.y * sin(angle),
   .y = v.x * sin(angle) + v.y * cos(angle),
 };
 return new_v;
}

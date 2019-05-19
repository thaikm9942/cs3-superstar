#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/polygon.h"
#include <math.h>

double polygon_area(List *polygon) {
  size_t size = list_size(polygon);
  double area = 0;

  // Applies the area formula to every pair of vertices except the last
  // and first one
  for(size_t i = 0; i < size; i++) {
    Vector v1 = *(Vector*)list_get(polygon, i % size);
    Vector v2 = *(Vector*)list_get(polygon, (i+1) % size);
    area = area + vec_cross(v1, v2);
  }

  // Calculates the final area
  area = area / 2;
  return area;
}

Vector polygon_centroid(List *polygon) {
  size_t size = list_size(polygon);
  double area = polygon_area(polygon);
  double sum_x = 0, sum_y = 0, cx, cy;

  // Applies the centroid formula to every pair of vertices except the last
  // and first one
  for(size_t i = 0; i < size; i++) {
    Vector v1 = *(Vector*)list_get(polygon, i % size);
    Vector v2 = *(Vector*)list_get(polygon, (i+1) % size);
    double cross_product = vec_cross(v1, v2);
    sum_x = sum_x + (v1.x + v2.x) * cross_product;
    sum_y = sum_y + (v1.y + v2.y) * cross_product;
  }

  // Calculates the coordinates for the cardiod
  cx = (double) sum_x / (6 * area);
  cy = (double) sum_y / (6 * area);

  Vector centroid = {
    .x = cx,
    .y = cy
  };

  return centroid;
}

void polygon_translate(List *polygon, Vector translation) {
  // Set the every vector in the data to be the original vector
  // translated by the translation vector
  for(size_t i = 0; i < list_size(polygon); i++){
    // The new vector
    Vector* ptr = list_get(polygon, i);
    *ptr = vec_add(*ptr, translation);
  }
}

void polygon_rotate(List *polygon, double angle, Vector point) {
  // We will translate all the current vertices by the inverse of the given point,
  // so that we can rotate all our vertices around the origin (0, 0)
  polygon_translate(polygon, vec_negate(point));
  for(size_t i = 0; i < list_size(polygon); i++){
    Vector* ptr = list_get(polygon, i);
    *ptr = vec_rotate(*ptr, angle);
  }
  // Now, we translate back all our vertices by the given point again to obtain
  // our desired mutant polygon
  polygon_translate(polygon, point);
}

#include <stdbool.h>
#include <assert.h>
#include "collision.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

struct bounds {
  double min;
  double max;
};

struct bounding_box {
  Bounds x_bounds;
  Bounds y_bounds;
};

struct projection_info {
  bool collided;
  double overlap;
};

BoundingBox *bounding_init(Bounds x_bounds, Bounds y_bounds){
  BoundingBox* bounding_box = malloc(sizeof(BoundingBox));
  assert(bounding_box != NULL);
  bounding_box->x_bounds = x_bounds;
  bounding_box->y_bounds = y_bounds;
  return bounding_box;
}

// Returns the unit vector of a given vector v
Vector unit_vector(Vector v){
  return vec_multiply(1.0 / vec_magnitude(v), v);
}

// Returns the normal vector of a given vector v
Vector normal_vector(Vector v){
  return (Vector){-v.y, v.x};
}

// Finds the boundaries (x_min, x_max and y_min, y_max) coordinates of a shape
BoundingBox *find_boundaries(List *shape){
  double x_min = INFINITY;
  double x_max = -INFINITY;
  double y_min = INFINITY;
  double y_max = -INFINITY;
  for(size_t i = 0; i < list_size(shape); i++){
    Vector point = *(Vector*)list_get(shape, i);
    if(point.x < x_min){
      x_min = point.x;
    }
    if(point.x > x_max){
      x_max = point.x;
    }
    if(point.y < y_min){
      y_min = point.y;
    }
    if(point.y > y_max){
      y_max = point.y;
    }
  }
  return bounding_init((Bounds){x_min, x_max}, (Bounds){y_min, y_max});
}

// Checks if there's any points in the bounds that overlap by checking whether
// the min or max of one bound lies between the min and max of the other.
// Returns true if the above condition is satisfied and false otherwise.
ProjectionInfo check_one_dimension_bounds(Bounds bounds1, Bounds bounds2){
  if(bounds1.min >= bounds2.min){
    if(bounds1.min <= bounds2.max){
      return (ProjectionInfo){true, bounds2.max - bounds1.min};
    }
    if(bounds1.max <= bounds2.max){
      return (ProjectionInfo){true, bounds1.max - bounds1.min};
    }
  }
  if(bounds2.min >= bounds1.min){
    if(bounds2.min <= bounds1.max){
      return (ProjectionInfo){true, bounds1.max - bounds2.min};
    }
    if(bounds2.max <= bounds1.max){
      return (ProjectionInfo){true, bounds2.max - bounds2.min};
    }
  }
  return (ProjectionInfo){false, 0};
}

// Checks to see if the BoundingBox of two objects intersect
bool check_bounds_collision(BoundingBox *box1, BoundingBox *box2){
  Bounds x_bounds1 = box1->x_bounds;
  Bounds y_bounds1 = box1->y_bounds;
  Bounds x_bounds2 = box2->x_bounds;
  Bounds y_bounds2 = box2->y_bounds;
  ProjectionInfo info1 = check_one_dimension_bounds(x_bounds1, x_bounds2);
  ProjectionInfo info2 = check_one_dimension_bounds(y_bounds1, y_bounds2);
  return info1.collided && info2.collided;
}

// Finds the projection of a shape on an axis by computing the dot product
// of each point with the axis and finding the minimum and maximum projection
// Returns a Bounds object.
Bounds find_projection(List *shape, Vector axis){
  double min = INFINITY;
  double max = -INFINITY;
  for(size_t i = 0; i < list_size(shape); i++){
    double p = vec_dot(axis, *(Vector*)list_get(shape, i));
    if(p < min){
      min = p;
    }
    if(p > max){
      max = p;
    }
  }
  return (Bounds){min, max};
}

// This function is used if the BoundingBox of each shape intersects with one
// another. Checks to see if any of the projections of the shapes overlap on all the
// axes (the normal vectors) produced by the edges. This function uses
// the separting axis theorem. Returns false if any of the projection does not
// overlap.
CollisionInfo check_projection_overlap(List* shape1, List* shape2){
  double min = INFINITY;
  Vector axis;
  for(size_t i = 0; i < list_size(shape1) - 1; i++) {
    Vector v1 = *(Vector*)list_get(shape1, i);
    Vector v2 = *(Vector*)list_get(shape1, i+1);
    Vector edge = vec_subtract(v1, v2);
    Vector normal = unit_vector(normal_vector(edge));
    Bounds proj1 = find_projection(shape1, normal);
    Bounds proj2 = find_projection(shape2, normal);
    ProjectionInfo proj_info = check_one_dimension_bounds(proj1, proj2);
    if(!proj_info.collided){
      return (CollisionInfo){false, 0, (Vector){0, 0}};
    }
    if(proj_info.overlap < min){
      min = proj_info.overlap;
      axis = normal;
    }
  }
  return (CollisionInfo){true, min, axis};
}

// Checks overlapping projections between two shapes on both the normal vectors
// produced by shape1 and shape2.
CollisionInfo check_overlap(List *shape1, List* shape2){
  CollisionInfo info1 = check_projection_overlap(shape1, shape2);
  Vector axis1 = info1.axis;
  CollisionInfo info2 = check_projection_overlap(shape2, shape1);
  Vector axis2 = info2.axis;
  if(info1.collided && info2.collided){
    if(info1.overlap < info2.overlap){
      return (CollisionInfo){true, info1.overlap, axis1};
    }
    else {
      return (CollisionInfo){true, info2.overlap, vec_negate(axis2)};
    }
  }
  return (CollisionInfo){false, 0, (Vector){0, 0}};
}

// The main collision detector.
// This function creates a rectangular bounding box based on the boundaries
// (x_min, x_max) and (y_min, y_max) of each shape, and then first checks if
// the bounding boxes intersects
// If the bounding boxes overlap, then the separate axis theorem is used to
// check for collision by checking if there exists an axis on which the
// projections of the two shapes do not overlap
CollisionInfo find_collision(List *shape1, List *shape2){
  BoundingBox *bounds1 = find_boundaries(shape1);
  BoundingBox *bounds2 = find_boundaries(shape2);
  bool bounds = check_bounds_collision(bounds1, bounds2);
  free(bounds1);
  free(bounds2);
  if(!bounds){
    return (CollisionInfo){false, 0, (Vector){0, 0}};
  }
  CollisionInfo info = check_overlap(shape1, shape2);
  if(!info.collided){
    return (CollisionInfo){false, 0, (Vector){0, 0}};
  }
  return (CollisionInfo){true, 0, info.axis};
}

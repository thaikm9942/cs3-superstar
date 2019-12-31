#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <stdbool.h>
#include "list.h"
#include "vector.h"
#include "body.h"

// A box representing the bounds of an object
typedef struct bounding_box BoundingBox;

// The bounds of one dimension of a box
typedef struct bounds {
  double min;
  double max;
} Bounds;

// The information about the projection
typedef struct projection_info ProjectionInfo;

// The information about the projection axis
typedef struct projection_info_axis ProjectionInfoAxis;

/**
 * Represents the status of a collision between two shapes.
 * The shapes are either not colliding, or they are colliding along some axis.
 */
typedef struct {
    /** Whether the two shapes are colliding */
    bool collided;
    /**
     * If the shapes are colliding, the axis they are colliding on.
     * This is a unit vector pointing from the first shape towards the second.
     * Normal impulses are applied along this axis.
     * If collided is false, this value is undefined.
     */
    double overlap;
    Vector axis;
} CollisionInfo;

// Initializes a bounding box with the given bounds
BoundingBox *bounding_init(Bounds x_bounds, Bounds y_bounds);

// Initializes a bounding box based on the coordinates of the shape
BoundingBox *find_boundaries(List *shape);

// Returns the x-bounds of a bounding box
Bounds get_x_bounds(BoundingBox *bounding_box);

// Returns the y-bounds of a bounding box
Bounds get_y_bounds(BoundingBox *bounding_box);

// Finds just the y bounds for a given shape
Bounds find_y_bounds(List *shape);

/**
 * Determines whether two convex polygons intersect.
 * The polygons are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding
 */
CollisionInfo find_collision(List *shape1, List *shape2);

#endif // #ifndef __COLLISION_H__

#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <stdbool.h>
#include "list.h"
#include "vector.h"
#include "body.h"

typedef struct bounding_box BoundingBox;
typedef struct bounds {
  double min;
  double max;
} Bounds;
typedef struct projection_info ProjectionInfo;
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

BoundingBox *bounding_init(Bounds x_bounds, Bounds y_bounds);
BoundingBox *find_boundaries(List *shape);
Bounds get_x_bounds(BoundingBox *bounding_box);
Bounds get_y_bounds(BoundingBox *bounding_box);
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

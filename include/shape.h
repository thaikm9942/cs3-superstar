#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdbool.h>
#include "body.h"
#include "sdl_wrapper.h"

/**
 * Represents a shape with a Body and number of lives. We're
 * assuming that the Shape can be anything
 */
typedef struct shape Shape;

/**
 * Initializes a rectangular platform as a Shape with a given dimension and color with its centroid at
 * positin and
 * @param position the center of the Shape to be drawn
 * @param dimension the width and the height of a rectangular Platform
 * @param color the color of Shape
 * @param life the number of lives the Shape has left
 */
Shape* platform_init(Vector position, Vector dimension, RGBColor color, int life);

Shape* star_init(Vector position, Vector dimension, RGBColor color, int life);

/**
 * Returns the body of a Shape
 * @param shape a pointer to a Shape
 */
Body* shape_get_body(Shape* shape);

/**
 * Returns the life of a Platform
 * @param shape a pointer to a Shape
 */
int shape_get_life(Shape* shape);

/**
 * Releases the memory allocated for a Shape
 * @param shape a pointer to a Shape to be freed
 */
void shape_free(Shape* shape);

/**
 * Draws the specified Shape using sdl_draw_polygon
 * @param shape a pointer to a Shape to be drawn
 */
void draw_shape(Shape* shape);

#endif // #ifndef __SHAPE_H__

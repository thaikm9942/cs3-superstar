#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdbool.h>
#include "body.h"
#include "sdl_wrapper.h"

// Defines BodyTypes
typedef enum {
    PLATFORM,
    PLAYER,
    SPIKE,
    POINT,
    GRAVITY_BALL,
    MOVING_BALL
} BodyType;

// Defines a body_info struct to be passed in Body
typedef struct body_info BodyInfo;

/* Initializes a star shape given a position vector, its dimensinos, color and
the number of lives */
BodyInfo* body_info_init(BodyType* type, size_t* life);

/**
 * Releases memory allocated for a BodyInfo
 * @param info a pointer to a BodyInfo to be free
 */
void body_info_free(BodyInfo* info);

/**
 * Returns the type of a Body from BodyInfo
 * @param info a pointer to a BodyInfo
 * @return a BodyType pointer
 */
BodyType* body_info_get_type(BodyInfo* info);

/**
 * Returns the number of lives left on a Body from BodyInfo
 * @param info a pointer to a BodyInfo
 * @return an int* pointer
 */
size_t *body_info_get_life(BodyInfo* info);

/**
 * Creates a Body with a star shape and given parameters
 * @param sides number of sides on the star
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
 * @returns a Body with star shape with centroid at position, mass, color and list_free
 */
Body *star_init(int sides, Vector position, double radius, double mass, RGBColor color, int life);

/**
 * Creates a Body with a block shape and given parameters
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
 */
Body *block_init(Vector position, Vector dimension, RGBColor color, int life);

/**
 * Creates a Body with a ball shape and given parameters
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
* @param type the BodyType of the ball (PLAYER, MOVING_BALL, GRAVITY_BALL)
 */
Body *ball_init(Vector position, double radius, double mass, RGBColor color, int life, BodyType* type);

Body *point_init(Vector position, double radius, double mass, RGBColor color, int life);
Body *gravity_ball_init(Vector position, double radius, double mass, RGBColor color, int life);
Body *moving_ball_init(Vector position, double radius, double mass, RGBColor color, int life);

/**
 * Creates a Body with a spike shape and given parameters representing SPIKE
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
 */

 Body *spike_init(Vector position, double radius, double mass, RGBColor color, int life);

 /**
  * Draws the specified Shape using sdl_draw_polygon
  * @param shape a pointer to a Shape to be drawn
  */

void draw_shape(Body* body);

#endif // #ifndef __SHAPE_H__

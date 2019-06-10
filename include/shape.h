#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdbool.h>
#include "body.h"
#include "sdl_wrapper.h"

// Defines BodyTypes
typedef enum {
    PLATFORM,
    // Special tag used for platform generation
    PLATFORM_TRIGGER,
    PLAYER,
    SPIKE,
    POINT,
    GRAVITY_BALL,
    MOVING_BALL,
    POWERUP_EXPAND,
    POWERUP_INVINCIBILITY,
    BOUND
} BodyType;

// Defines a body_info struct to be passed in Body
typedef struct body_info BodyInfo;

/* Initializes a star shape given a position vector, its dimensinos, color and
the number of lives */
BodyInfo* body_info_init(BodyType* type, size_t life);

/**
 * Releases memory allocated for a BodyInfo
 * @param info a pointer to a BodyInfo to be free
 */
void body_info_free(BodyInfo* info);

/**
 * Returns the type of a Body from BodyInfo
 * @param info a pointer to a BodyInfo
 * @return a BodyType
 */
BodyType body_info_get_type(BodyInfo* info);

/**
 * Returns the number of lives left on a Body from BodyInfo
 * @param info a pointer to a BodyInfo
 * @return an int* pointer
 */
size_t body_info_get_life(BodyInfo* info);

/**
 * Returns whether or not the body is in collision
 * @param info a pointer to a BodyInfo
 * @return TRUE or FALSE depending on whether the Body is in collision
 */
bool body_info_get_collision(BodyInfo* info);

/**
 * Sets the number of lives left on a Body to a new one.
 * @param info a pointer to a BodyInfo
 * @param new_life a new number of lives to be set
 */
void body_info_set_life(BodyInfo* info, size_t new_life);

/**
 * Sets whether or not the body is currently in collision with another body
 * @param info a pointer to a BodyInfo
 * @param colliding either TRUE or FALSE will be set here
 */
void body_info_set_collision(BodyInfo* info, bool colliding);

/**
 * Creates a Body with a star shape and given parameters
 * @param sides number of sides on the star
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
 * @param type the BodyType of the star
 * @returns a Body with star shape of specified type with centroid at position, mass, color and
 * number of lives
 */
Body *star_init(int sides, Vector position, double radius, double mass, RGBColor color, size_t life, BodyType* type);

// Calls on star_init to create a PLAYER type star
Body *player_init(int sides, Vector position, double radius, double mass, RGBColor color, size_t life);

/**
 * Creates a Body with a block shape and given parameters
 * @param position the position to translate the block to after
 * @param radius radius of the block
 * @param mass the mass of the block
 * @param RGBColor the color of the block
 * @param life the number of lives the block has
 * @param isTrigger a boolean indicating whether the platform triggers the
 * next generation of platform or not
 * @returns a Body with a block shape with centroid at position, mass, color and
 * number of lives
 */
Body *block_init(Vector position, Vector dimension, RGBColor color, size_t life, bool isTrigger);

/**
 * Creates a Boundary Body with a block shape and given parameters
 * @param position the position to translate the block to after
 * @param radius radius of the block
 * @param mass the mass of the block
 * @param RGBColor the color of the block
 * @param life the number of lives the block has
 * @returns a Body with a block shape with centroid at position, mass, color and
 * number of lives
 */
Body *boundary_init(Vector position, Vector dimension, RGBColor color, size_t life);

/**
 * Creates a Body with a ball shape and given parameters
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
* @param type the BodyType of the ball (PLAYER, MOVING_BALL, GRAVITY_BALL)
 */
Body *ball_init(Vector position, double radius, double mass, RGBColor color, size_t life, BodyType* type);

// Initializes a POINT type ball using ball_init
Body *point_init(Vector position, double radius, double mass, RGBColor color, size_t life);

// Initializes a GRAVITY_BALL type hazard ball using ball_init
Body *gravity_ball_init(Vector position, double radius, double mass, RGBColor color, size_t life);

// Initializes a MOVING_BALL type hazard ball using ball_init
Body *moving_ball_init(Vector position, double radius, double mass, RGBColor color, size_t life);

/**
 * Creates a Body with a spike shape and given parameters representing SPIKE type
 * hazard
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @param life the number of lives the star has
 */
Body *spike_init(Vector position, double radius, double mass, RGBColor color, size_t life);

 /**
  * Draws the specified Shape using sdl_draw_polygon
  * @param shape a pointer to a Shape to be drawn
  */
void draw_shape(Body* body);

#endif // #ifndef __SHAPE_H__

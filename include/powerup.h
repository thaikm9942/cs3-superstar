#ifndef __POWERUP_H__
#define __POWERUP_H__

#include <stdbool.h>
#include "forces_game.h"
#include "sdl_wrapper.h"

/**
 * Creates a Body with a 5-sided star shape and given parameters representing a
 * POWERUP_INVINCIBILITY powerup
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @returns a pointer to a Body with star shape drawn using Radius with
 * centroid at position, mass, color
 */
Body *invincibility_init(Vector position, double radius, double mass, RGBColor color);

/**
 * Creates a Body with a 5-sided star shape and given parameters representing a
 * POWERUP_EXPAND powerup
 * @param position the position to translate the shape to after
 * @param radius radius of the star
 * @param mass the mass of the star
 * @param RGBColor the color of the star
 * @returns a pointer to a Body with star shape drawn using Radius with
 * centroid at position, mass, color
 */
Body *expand_init(Vector position, double radius, double mass, RGBColor color);

/**
 * Creates a partial destructive collision between player and powerup stars that
 * activates the statuses on the status board in scene depending on what BodyType
 * the collided powerup is
 * @param scene the scene containing the bodies
 * @param player a Body pointer to the player
 * @param powerup a Body pointer to the powerup
 */
void create_player_powerup_collision(Scene *scene, Body *player, Body *powerup);

#endif // #ifndef __POWERUP_H__

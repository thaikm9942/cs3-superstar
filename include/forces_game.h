#ifndef __FORCES_GAME_H__
#define __FORCES_GAME_H__

#include "forces.h"
#include "shape.h"
#include "collision.h"

/* ALL SUPERSTAR FUNCTIONS */
void create_gravity(Scene *scene, Body *player);
void create_special_collision(Scene *scene, Body *player, Body *platform,
CollisionHandler handler, void *aux, FreeFunc freer);
void create_player_platform_collision(Scene *scene, Body* player, Body* platform);
void create_partial_destructive_collision_with_life(Scene *scene, Body *object, Body *target);
void create_partial_collision_with_life(Scene *scene, double elasticity, Body *body, Body *target);
void create_player_point_collision(Scene *scene, Body* player, Body* point);
void create_player_gravity_collision(Scene *scene, double elasticity, Body* player, Body* grav_ball);
#endif // #ifndef __FORCES_GAME_H__

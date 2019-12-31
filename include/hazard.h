#ifndef __HAZARD_H__
#define __HAZARD_H__

#include "body.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "forces_game.h"

void spike_hazard_init(Vector position, Scene* scene);
//partial destroy_body collision type if out of Lives, otherwise, repel_body
//Empty force exerted (does not exert a force)

void gravity_hazard_init(Vector position, Scene* scene);
//Empty collision type (only collides with spikes, to be destroyed)
//Exerts gravity on the player

void moving_ball_hazard_init(Vector position, Vector velocity, double mass, Scene* scene);
//physics_collision collision type (spikes still destroy this and are unmoved)
//Empty force exerted (does not exert a force)

void spring_platforms_hazard_init(Vector position1, Vector position2, Vector dimension, double k, Scene* scene);
//repel_body collision type, if both platforms touching the player, partial destroy_body
//Spring force exerted on the other platform (generated in a pair)

#endif // #ifndef __HAZARD_H__

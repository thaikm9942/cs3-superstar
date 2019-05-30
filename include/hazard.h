#ifndef __HAZARD_H__
#define __HAZARD_H__

#include <stdbool.h>
#include "body.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "forces.h"

//A hazard contains a shape, a collision mechanic, and a force
typedef struct hazard {
    Shape* shape;
    CollisionHandler on_collide;
    ForceCreator force_type;
} Hazard;

Hazard* spike_init(Vector position);
//partial destroy_body collision type if out of Lives, otherwise, repel_body
//Empty force exerted (does not exert a force)

Hazard* gravity_well_init(Vector position, int mass, int radius);
//Empty collision type (only collides with spikes, to be destroyed)
//Exerts gravity on the player

Hazard* moving_ball_init(Vector position, Vector velocity, int mass, int radius);
//physics_collision collision type (spikes still destroy this and are unmoved)
//Empty force exerted (does not exert a force)

Hazard* spring_platforms_init(Vector position, Vector dimension, int k);
//repel_body collision type, if both platforms touching the player, partial destroy_body
//Spring force exerted on the other platform (generated in a pair)

#endif // #ifndef __HAZARD_H__
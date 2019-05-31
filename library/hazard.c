#include <stdbool.h>
#include "body.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "forces.h"
#include "hazard.h"

//Constants:

//HAZARD_RADIUS
//SPIKE_COLOR
//HAZARD_MASS
//GRAV_COLOR
//G

void spike_hazard_init(Vector position, Scene* scene){
    Body* spike_body = malloc(sizeof(Body);
    spike_body = spike_init(position, HAZARD_RADIUS, INFINITY, SPIKE_COLOR, INFINITY);
    scene_add_body(spike_body, scene)
}

void gravity_well_init(Vector position, double mass, Body* player, Scene* scene){
    Body* grav_body = malloc(sizeof(Body);
    grav_body = hazard_init(position, HAZARD_RADIUS, HAZARD_MASS, GRAV_COLOR, 1);
    scene_add_body(grav_body, scene);
    create_newtonian_gravity(scene, G, grav_body, player);
}

void moving_ball_init(Vector position, Vector velocity, double mass, Scene* scene){
    Body* moving_ball_body = malloc(sizeof(Body));
    moving_ball_body = moving_ball_hazard_init(position, HAZARD_RADIUS, mass, BALL_COLOR, 1);
    scene_add_body(moving_ball_body, scene);
}
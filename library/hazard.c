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

Body* spike_hazard_init(Vector position){
    Body* spike_body = malloc(sizeof(Body);
    spike_init(position, HAZARD_RADIUS, INFINITY, SPIKE_COLOR, INFINITY);
}

void gravity_well_init(Vector position, double mass, Body* player, Scene* scene){
    Body* grav_body = malloc(sizeof(Body);
    hazard_init(position, HAZARD_RADIUS, HAZARD_MASS, GRAV_COLOR, 1);
    create_newtonian_gravity(scene, G, grav_body, player);
}
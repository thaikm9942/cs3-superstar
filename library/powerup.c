#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "powerup.h"

// CollisionHandler that is used by the ForceCreator to handle collisions between
// player and powerup stars
void activate_powerup(Body* player, Body* powerup, Vector axis, void* aux){
  Scene* scene = (Scene*) aux;
  Status* status = scene_get_status(scene);
  BodyInfo* powerup_info = body_get_info(powerup);
  if(body_info_get_type(powerup_info) == POWERUP_EXPAND){
    activate_expand(status);
  }
  if(body_info_get_type(powerup_info) == POWERUP_INVINCIBILITY){
    activate_invincibility(status);
  }
  body_remove(powerup);
}

Body *invincibility_init(Vector position, double radius, double mass, RGBColor color){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = POWERUP_INVINCIBILITY;
  return star_init(5, position, radius, mass, color, 0, type);
}

Body *expand_init(Vector position, double radius, double mass, RGBColor color){
  BodyType *type = malloc(sizeof(*type));
  assert(type != NULL);
  *type = POWERUP_EXPAND;
  return star_init(5, position, radius, mass, color, 0, type);
}

void create_player_powerup_collision(Scene *scene, Body *player, Body *powerup){
  create_collision(scene, player, powerup, (CollisionHandler) activate_powerup, (void*) scene, NULL);
}

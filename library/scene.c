#include "scene.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "status.h"
const size_t INITIAL_SIZE = 10;

struct scene_forcer {
  ForceCreator forcer;
  void *aux;
  FreeFunc freer;
  List* bodies_affected;
};

struct scene {
  List* bodies;
  List* scene_forcers;
  Status* status;
  size_t score;
};

Scene *scene_init(void) {
  Scene* scene = malloc(sizeof(Scene));
  assert(scene != NULL);
  List* bodies = list_init(INITIAL_SIZE, (FreeFunc) body_free);
  assert(bodies != NULL);
  List* scene_forcers = list_init(INITIAL_SIZE * 5, NULL);
  assert(scene_forcers != NULL);
  scene->bodies = bodies;
  scene->scene_forcers = scene_forcers;
  scene->status = status_init();
  scene->score = 0;
  return scene;
}

SceneForcer *scene_forcer_init(ForceCreator forcer, void *aux, FreeFunc freer, List* bodies_affected){
  SceneForcer *scene_forcer = malloc(sizeof(SceneForcer));
  scene_forcer->forcer = forcer;
  scene_forcer->aux = aux;
  scene_forcer->freer = freer;
  scene_forcer->bodies_affected = bodies_affected;
  return scene_forcer;
}

// Frees a single scene_forcer; the List of bodies_affected should
void scene_forcer_single_free(SceneForcer* scene_forcer){
  if(scene_forcer->freer != NULL){
    scene_forcer->freer(scene_forcer->aux);
  }
  list_free(scene_forcer->bodies_affected);
  free(scene_forcer);
}

// THIS SHOULD NOT FREE ANY BODIES THAT ARE MARKED FOR REMOVAL
void scene_forcer_free(Scene* scene){
  SceneForcer* forcer;
  for(size_t i = 0; i < list_size(scene->scene_forcers); i++){
    forcer = (SceneForcer*) list_get(scene->scene_forcers, i);
    scene_forcer_single_free(forcer);
  }
}

void scene_free(Scene *scene) {
  list_free(scene->bodies);
  scene_forcer_free(scene);
  list_free(scene->scene_forcers);
  // Frees status board
  status_free(scene->status);
  free(scene);
}

size_t scene_bodies(Scene *scene) {
  return list_size(scene->bodies);
}

// Returns a status board keep tracking of the powerups active
Status* scene_get_status(Scene *scene){
  return scene->status;
}

Body *scene_get_body(Scene *scene, size_t index) {
  assert(index < scene_bodies(scene));
  return list_get(scene->bodies, index);
}

size_t scene_get_score(Scene *scene) {
  return scene->score;
}

void scene_set_score(Scene *scene, size_t new_score) {
  scene->score = new_score;
}


void scene_add_body(Scene *scene, Body *body) {
  list_add(scene->bodies, body);
}

void scene_add_bodies_force_creator(
    Scene *scene, ForceCreator forcer, void *aux, List *bodies, FreeFunc freer
){
  list_add(scene->scene_forcers, scene_forcer_init(forcer, aux, freer, bodies));
}

void scene_add_force_creator(Scene *scene, ForceCreator forcer, void *aux, FreeFunc freer){
  scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_remove_body(Scene *scene, size_t index) {
  assert(index < scene_bodies(scene));
  body_remove((Body*)list_get(scene->bodies, index));
}

void scene_tick(Scene *scene, double dt) {
  // Iterate over every force creator
  for(size_t i = 0; i < list_size(scene->scene_forcers); i++){
    SceneForcer* scene_forcer = (SceneForcer*) list_get(scene->scene_forcers, i);
    scene_forcer->forcer(scene_forcer->aux);
  }
  for(size_t i = 0; i < list_size(scene->scene_forcers); i++){
    SceneForcer* scene_forcer = (SceneForcer*) list_get(scene->scene_forcers, i);
    List* affected = scene_forcer->bodies_affected;
    // Checks if any of the affected bodies have been marked for removal. If yes,
    // then set local variable to true to avoid calling scene_forcer onto
    for(size_t j = 0; j < list_size(affected); j++){
      if(body_is_removed((Body*)list_get(affected, j))){
        scene_forcer_single_free(list_remove(scene->scene_forcers, i));
        i--;
        break;
      }
    }
  }
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body *body = scene_get_body(scene, i);
    body_tick(body, dt);
  }
  for(size_t i = 0; i < scene_bodies(scene); i++){
    Body *body = scene_get_body(scene, i);
    if(body_is_removed(body)){
      body_free(list_remove(scene->bodies, i));
      i--;
    }
  }
  status_tick(scene_get_status(scene));
}

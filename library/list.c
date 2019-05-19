#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/list.h"
#include <assert.h>

struct list {
  void ** data;
  size_t capacity;
  size_t size;
  FreeFunc freer;
};

const size_t GROWTH_FACTOR = 5;

List *list_init(size_t initial_size, FreeFunc freer) {
  List *list = malloc(sizeof(List));
  assert(list != NULL);
  list->capacity = initial_size;
  list->size = 0;
  list->data = (void **) malloc(initial_size * sizeof(void *));
  assert(list->data != NULL);
  list->freer = freer;
  return list;
}

void list_free(List *list) {
  if(list->freer != NULL) {
    for(size_t i = 0; i < list_size(list); i++){
      list->freer(list->data[i]);
    }
  }
  free(list->data);
  free(list);
}

size_t list_size(List *list){
  return list->size;
}

void *list_get(List *list, size_t index){
  assert(index < list_size(list));
  return list->data[index];
}

void *list_remove(List *list, size_t index){
  assert(list_size(list) != 0);
  void *removed = list_get(list, index);
  for(size_t i = index + 1; i < list_size(list); i++)
  {
    list->data[i - 1] = list->data[i];
  }
  list->size--;
  return removed;
}

/** Checks the current capacity of List and shows whether it is full.
 * Returns 0 if false.
 * Returns 1 if true.
 */
size_t list_is_full(List *list) {
  return list_size(list) + 1 > list->capacity;
}

void list_resize(List *list) {
  list->capacity = list->capacity * GROWTH_FACTOR;
  list->data = realloc(list->data, list->capacity * sizeof(void *));
  assert(list->data != NULL);
}

void list_add(List *list, void *value) {
  if(list_is_full(list) == 1){
    list_resize(list);
  }
  list->data[list_size(list)] = value;
  list->size++;
}

void list_set(List* list, size_t index, void *value) {
  assert(index < list_size(list));
  if(list_get(list, index) != NULL){
    list->freer(list_get(list, index));
  }
  list->data[index] = value;
}

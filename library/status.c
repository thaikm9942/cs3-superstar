#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "status.h"

struct status {
  bool isInvincible;
  bool isExpanded;
};

const double DT = 1;

Status* status_init(void){
  Status* status = malloc(sizeof(Status));
  assert(status != NULL);
  status->isInvincible = false;
  status->isExpanded = false;
  return status;
}

void status_free(Status* status){
  free(status);
}

void deactivate_invincibility(Status* status){
  status->isInvincible = false;
}

void activate_invincibility(Status* status){
  status->isInvincible = true;
}

void activate_expand(Status* status){
  status->isExpanded = true;
}

void deactivate_expand(Status* status){
  status->isExpanded = false;
}

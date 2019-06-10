#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "status.h"

const int DT = 1;

Status* status_init(void){
  Status* status = malloc(sizeof(Status));
  assert(status != NULL);
  status->isInvincible = false;
  status->timeInv = 0;
  status->isExpanded = false;
  status->timeExp = 0;
  return status;
}

void status_free(Status* status){
  free(status);
}

void deactivate_invincibility(Status* status){
  status->isInvincible = false;
}

void activate_invincibility(Status* status, int time){
  status->isInvincible = true;
  status->timeInv = time * DT;
}

void status_tick(Status* status){
  if(status->timeInv > 0){
    status->timeInv--;
  }
  else{
    deactivate_invincibility(status);
  }
  if(status->timeExp > 0){
    status->timeExp--;
  }
  else{
    deactivate_expand(status);
  }
}

void activate_expand(Status* status, int time){
  status->isExpanded = true;
  status->timeExp = time * DT;
}

void deactivate_expand(Status* status){
  status->isExpanded = false;
}

#ifndef __STATUS_H__
#define __STATUS_H__

/**
 * Represents a Status board that keeps track of the status of Powerups. Scene
 * will store a Status board to be used to apply powerups to the corresponding
 * bodies in the scene.
 */

//typedef struct status Status;
typedef struct status {
  bool isInvincible;
  int timeInv;
  bool isExpanded;
  int timeExp;

} Status;

// An initializer for a status board
Status* status_init(void);

/**
 * Releases allocated memory for the Status pointer
 * @param status pointer to Status to be freed
 */
void status_free(Status* status);

// Activates invincibility
void activate_invincibility(Status* status, int time);

// Deactivates invincibility
void deactivate_invincibility(Status* status);

// Activates platform expansion
void activate_expand(Status* status, int time);

// Deactivates platform expansion
void deactivate_expand(Status* status);

#endif // #ifndef __STATUS_H_

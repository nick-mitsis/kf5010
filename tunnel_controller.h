#ifndef _TUNNEL_CONTROLLER_H
#define _TUNNEL_CONTROLLER_H

typedef enum {NORTH, SOUTH} direction_t;

/**
 * @brief controls entry to the tunnel
 * @param dir the direction of travel when entering
 */
void enter_tunnel(direction_t dir);

/**
 * @brief controls exit from the tunnel
 * @param dir the direction of travel when leaving
 */
void exit_tunnel(direction_t dir);

/**
 * @brief Gives the opposite direction to its argument
 * @param dir a direction, either NORTH or SOUTH
 * @returns the opposite of direction dir
 */
direction_t opposite(direction_t dir);

#endif

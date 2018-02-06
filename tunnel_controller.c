#include <assert.h>
#include <pthread.h>
#include "tunnel_controller.h"
#include "console.h"

static int n_in_tunnel[2];
static unsigned long n_total[2];
static bool queue[2];
static int total_opp_wait[2];

// initialise mutexes
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// initialise condition variables
pthread_cond_t free_slot = PTHREAD_COND_INITIALIZER;

/**
 * @brief returns true if car is not allowed to enter
 *        the tunnel and false if it is allowed
 * @param dir the direction of travel when entering
 */
bool car_not_allowed(direction_t dir) {
    if (n_in_tunnel[opposite(dir)] != 0 || n_in_tunnel[dir] == 3 || total_opp_wait[dir] == 6) {
		return true;
    }
    else {
		return false;
    }
}

/**
 * @brief controls entry to the tunnel
 * @param dir the direction of travel when entering
 */
void enter_tunnel(direction_t dir) {
    pthread_mutex_lock(&lock); // lock the mutex

    queue[dir] = true; // incoming car

    // While one of three conditions is true, the car must wait
    while (car_not_allowed(dir)) {
		pthread_cond_wait(&free_slot, &lock);
    }

    // Add cars to waiting queue
    if (queue[opposite(dir)]) {
		total_opp_wait[dir] += 1;
		total_opp_wait[opposite(dir)] = 0;
    }


    // If all of three cond. are false the car enters the tunnel
    n_in_tunnel[dir] += 1; // car enters tunnel

    queue[dir] = false; 
    

    // Update lcd
    lcd_write_at(4+dir, 0, "In tunnel (%s) : %2d",
            (dir == NORTH) ? "NORTH" : "SOUTH", n_in_tunnel[dir]);

    lcd_write_at(10+NORTH, 0, "QUEUE (%s) : %2d", "NORTH", queue[NORTH]);
    lcd_write_at(10+SOUTH, 0, "QUEUE (%s) : %2d", "SOUTH", queue[SOUTH]);

    lcd_write_at(15+NORTH, 0, "CARS AFTER APP. OF CAR ON OPP.DIR (%s) : %2d", "NORTH", total_opp_wait[NORTH]);
    lcd_write_at(15+SOUTH, 0, "CARS AFTER APP. OF CAR ON OPP.DIR (%s) : %2d", "SOUTH", total_opp_wait[SOUTH]);


    // If conditions are false the programm fails
    assert(n_in_tunnel[opposite(dir)] == 0); // cars from both dirs into tunnel
    assert(n_in_tunnel[dir] <= 3); // cars on one dir exceeded three
    assert(total_opp_wait[dir] <= 6); // cars passing through dir while opp. wait, exceed 6


    pthread_mutex_unlock(&lock); // unlock mutex
}

/**
 * @brief controls exit from the tunnel
 * @param dir the direction of travel when leaving
 */
void exit_tunnel(direction_t dir) {
    pthread_mutex_lock(&lock); // lock the mutex
	
    n_in_tunnel[dir] -= 1; // Car exits tunnel
    n_total[dir] += 1; // and added to total

    // Update lcd
    lcd_write_at(4+dir, 0, "In tunnel (%s) : %2d",
            (dir == NORTH) ? "NORTH" : "SOUTH", n_in_tunnel[dir]);
    lcd_write_at(6+dir, 0, "Total     (%s) : %d",
            (dir == NORTH) ? "NORTH" : "SOUTH", n_total[dir]);

    lcd_write_at(10+NORTH, 0, "QUEUE (%s) : %2d", "NORTH", queue[NORTH]);
    lcd_write_at(10+SOUTH, 0, "QUEUE (%s) : %2d", "SOUTH", queue[SOUTH]);

    lcd_write_at(15+NORTH, 0, "CARS AFTER APP. OF CAR ON OPP.DIR (%s) : %2d", "NORTH", total_opp_wait[NORTH]);
    lcd_write_at(15+SOUTH, 0, "CARS AFTER APP. OF CAR ON OPP.DIR (%s) : %2d", "SOUTH", total_opp_wait[SOUTH]);

    pthread_cond_broadcast(&free_slot); // wake up all threads(cars)

    pthread_mutex_unlock(&lock); // unlock mutex
}

/**
 * @brief Gives the opposite direction to its argument
 * @param dir a direction, either NORTH or SOUTH
 * @returns the opposite of direction dir
 */
direction_t opposite(direction_t dir) {
    return ((dir == NORTH) ? SOUTH : NORTH);
}

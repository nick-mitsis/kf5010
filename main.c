#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#include "console.h"
#include "tunnel_controller.h"

/**
 * @brief some constant definitions
 */
enum {
    N_VEHICLES = 20,         /** number of vehicles in the system */
    APPROACH_LB = 1000000,   /** lower bound on time of approach to tunnel */
    APPROACH_UB = 3000000,   /** upper bound on time of approach to tunnel */
    IN_TUNNEL_LB = 500000,   /** lower bound on time travelling in tunnel */
    IN_TUNNEL_UB = 1500000,  /** upper bound on time travelling in tunnel */
};

static void *vehicle_thr(void *arg);
unsigned int scale_random(double r, int l, int u);

/**
 * @brief Creates the vehicle threads and then loops forever
 */
int main (void) {
    int rc;
    unsigned long i;
    pthread_t thread[N_VEHICLES];

    console_init();
    lcd_write_at(0, 0, "THE OTHER TYNE TUNNEL");
    lcd_write_at(1, 0, "---------------------");

    for (i = 0; i < N_VEHICLES; i += 1) {
        rc = pthread_create(&thread[i], NULL, vehicle_thr, (void *)i);
        assert(rc == 0);
    }

    while (true) {
        /* skip */
    }
    exit(0);
}

/**
 * @brief Thread to simulate the behaviour of a vehicle
 * @param arg the id of the thread
 */
static void *vehicle_thr(void *arg) {
    double r;
    unsigned long id = (unsigned long)arg;
    struct drand48_data rand_state;
    int rc;
    direction_t dir;

    /* initialise random seed */
    rc = srand48_r((long)id * 10 + time(NULL), &rand_state);
    assert(rc == 0);

    while (true) {
        /* pick a direction of travel at random */
        rc = drand48_r(&rand_state, &r);
        assert(rc == 0);
        if (scale_random(r, 0, 1) == 0) {
            dir = NORTH;
        } else {
            dir = SOUTH;
        }

        /* approach tunnel */
        rc = drand48_r(&rand_state, &r);
        assert(rc == 0);
        usleep(scale_random(r, APPROACH_LB, APPROACH_UB));

        /* enter tunnel */
        enter_tunnel(dir);

        /* travel through tunnel */
        rc = drand48_r(&rand_state, &r);
        assert(rc == 0);
        usleep(scale_random(r, IN_TUNNEL_LB, IN_TUNNEL_UB));

        /* exit tunnel */
        exit_tunnel(dir);
    }
}

/**
 * @brief Scale r to an integer in the range [l,u)
 * @param r a random double in the range [0, 1)
 * @param l the lower bound of the range
 * @param u the upper bound of the range
 */
unsigned int scale_random(double r, int l, int u) {
    assert((0 <= r) && (r < 1) && (0 <= l) && (l < u));
    return (unsigned int)(lround(l + ((u - l) * r)));
}

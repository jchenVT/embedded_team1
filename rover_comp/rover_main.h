/*
 * star.h
 *
 *  Created on: Feb 5, 2020
 *      Author: giffen
 */

#ifndef ROVER_MAIN_H_
#define ROVER_MAIN_H_

#include <rover_queues.h>
#include <rover_uart.h>
#include "sensor_state.h"

#define KP  0.2
#define KI  0.1

struct PIDvalues {
    long desiredTicks;
    long currentTicks;
    long integral;
    long prevError;

    char direction;
};

void *mainThread(void *arg0);

#endif /* ROVER_MAIN_H_ */

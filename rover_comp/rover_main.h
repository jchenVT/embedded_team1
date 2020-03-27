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

#define KP  4
#define KI  3

struct PIDvalues {
    char desiredSpeed;
    char currentSpeed;
    long integral;
    long prevError;
};

void *mainThread(void *arg0);

#endif /* ROVER_MAIN_H_ */

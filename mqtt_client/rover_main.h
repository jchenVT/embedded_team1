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

#define KP  0.4
#define KI  0.1

/*
 * - Plot!
 * - Move motor commands
 * - Slightly larger timeframe to read encoders
 * - Don't test from board, try a python script instead
 *      It shouldn't be on the board anyways
 */

struct PIDvalues {
    long desiredTicks;
    long currentTicks;
    long integral;
    long prevError;

    char direction;
};

enum roverStates {stop, turn_left, turn_right, move_forward, target};

void *mainRoverThread(void *arg0);
void timerCallbackDebug(TimerHandle_t xTimer);
void timerCallbackTesting(TimerHandle_t x_Timer);

#endif /* ROVER_MAIN_H_ */

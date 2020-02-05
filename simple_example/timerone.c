/*
 * timerone.c
 *
 *  Created on: Feb 3, 2020
 *      Author: chris
 */

#include "timerone.h"

#define TIMER_LENGTH 1000

void * mainTimerOneThread() {
    Timer_Handle timer1;
    Timer_Params params;

    Timer_init();
    Timer_Params_init(&params);

    params.periodUnits = Timer_PERIOD_HZ;
    params.period = 1;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerSecondCallback;

    timer1 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer1 == NULL) {
        /* Failed to initialized timer */
        while (1) {}
    }

    if (Timer_start(timer1) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1) {}
    }

    return (NULL);
}

void timerSecondCallback() {
    sendTimeMsgToQ1(TIMER_LENGTH);
}

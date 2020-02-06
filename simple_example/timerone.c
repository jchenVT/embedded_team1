/*
 * timerone.c
 *
 *  Created on: Feb 3, 2020
 *      Author: chris
 */

#include "timerone.h"

#define TIMER_LENGTH 1000

void *mainTimerOneThread(void *arg0) {
    Timer_Handle timer1;
    Timer_Params params;

    /**************************/
    dbgOutputLoc(T1_TIMER_INITIALIZE);
    /**************************/

    Timer_init();
    Timer_Params_init(&params);

    params.periodUnits = Timer_PERIOD_HZ;
    params.period = 1;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerSecondCallback;

    timer1 = Timer_open(CONFIG_TIMER_0, &params);

    /**************************/
    dbgOutputLoc(T1_TIMER_OPENED);
    /**************************/

    if (timer1 == NULL) {
        /* Failed to initialized timer */
        stop_all();
    }

    if (Timer_start(timer1) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        stop_all();
    }

    return (NULL);
}

void timerSecondCallback() {

    /**************************/
    dbgOutputLoc(T1_CALLBACK);
    /**************************/

    sendTimeMsgToQ1(TIMER_LENGTH);
}

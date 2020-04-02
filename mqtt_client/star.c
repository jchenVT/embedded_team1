/*
 * star.c
 *
 *  Created on: Mar 31, 2020
 *      Author: giffen
 */

#include "star.h"

/*
 *  @function   mainThread
 *              Main thread that will pull from the sensor queue to
 *              change the FSM states.
 *
 *  @params     arg0
 *  @return     void
 */
void *starThread(void *arg0) {

    /* Set up software timer (100ms -> 10 msgs a second)*/
    TimerHandle_t timerPub = xTimerCreate("PublishTimer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timerCallback);
    xTimerStart(timerPub, 0);

    struct qRoverMsg data = {0};

    while(1) {

        // blocking read on subQ
        receiveFromSubRoverQ(&data);

        if (data.state == 1) {
            UART_PRINT("Subscription successfully received! \n\r");
        }
        else {
            UART_PRINT("[ERROR]: Subscription unsuccessfully received... \n\r");
        }
    }
}

/*
 *  @function   timerCallback
 *              Timer callback that will publish to a topic
 *              periodically.
 *
 *  @params
 *  @return     void
 */
void timerCallback(TimerHandle_t xTimer) {

    // publish to message queue
    if (packageRoverJSON(1) == 1) {
        UART_PRINT("Publish request sent! \n\r");
    }
    else {
        UART_PRINT("[ERROR]: Publish request  not sent... \n\r");
    }

}

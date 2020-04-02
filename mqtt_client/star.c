/*
 * star.c
 *
 *  Created on: Mar 31, 2020
 *      Author: giffen
 */

#include "star.h"

static int attemptPubCount;
static int recvSubCount;
static bool status;
static char subTopic[];
static char pubTopic[];


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

    /* Set up software timer to send statistics ever 10 seconds*/
    TimerHandle_t timerDebug = xTimerCreate("PublishTimer", pdMS_TO_TICKS(10000), pdTRUE, NULL, timerCallbackDebug);
    xTimerStart(timerDebug, 0);

    /* Declare statistics variable */
    attemptPubCount = 0;
    recvSubCount = 0;
    status = true; // working
    subTopic = "rover";
    pubTopic = "arm_sensor"

    struct qRoverMsg data = {0};

    while(1) {

        // blocking read on subQ
        receiveFromSubRoverQ(&data);

        if (data.state == 1) {
            UART_PRINT("Subscription successfully received! \n\r");
            recvSubCount++;
        }
        else {
            UART_PRINT("[ERROR]: Subscription unsuccessfully received... \n\r");
            status = false;
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

    int sensorID = 0;
    int sensorVal = 1;

    // publish to message queue
    if (packageArmSensorJSON(sensorID, sensorVal) == 1) {
        UART_PRINT("Publish request sent! \n\r");
        attemptPubCount++;
    }
    else {
        UART_PRINT("[ERROR]: Publish request not sent... \n\r");
        status = false;
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
void timerCallbackDebug(TimerHandle_t xTimer) {

    // publish to message queue
    if (packageDebugJSON(attemptPubCount, recvSubCount, status, subTopic, pubTopic) == 1) {
        UART_PRINT("Published statistics \n\r");
    }
    else {
        UART_PRINT("[ERROR]: Publish request not sent... \n\r");
    }
}


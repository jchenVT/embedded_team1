/*
 * sensor_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu
 */

#include "sensor_queue.h"

/*
 *  @function   createQ1
 *              Wrapper for the RTOS function to create a queue. Uses
 *              the queue variables from sensor_queue.h to define the
 *              amount of memory the queue will use.
 *
 *  @params     None
 *  @return     bool - indicates whether the message queue was created
 */
bool createQ1() {

    msgQ = xQueueCreate( qLENGTH, qITEMSIZE);

    return msgQ == NULL ? false : true;
}

/*
 *  @function   sendTimeMsgToQ1
 *              Wrapper for the RTOS function to send time data through
 *              the queue. Should be called from the callbacks (via ISR).
 *              Refer to page 110 of RTOS doc.
 *
 *  @params     timeVal - value from timers
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendTimeMsgToQ1(unsigned int timeVal) {

    long long int newMsg = TIME_DATA | timeVal;

    return xQueueSendToBackFromISR( msgQ, &newMsg, 0 );
}

/*
 *  @function   sendSensorMsgToQ1
 *              Wrapper for the RTOS function to send sensor data through
 *              the queue. Should be called from the callbacks (via ISR).
 *              Refer to page 110 of RTOS doc.
 *
 *  @params     mmDist - distance values
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendSensorMsgToQ1(int mmDist) {

    long long int newMsg = SENSOR_DATA | mmDist;

    return xQueueSendToBackFromISR( msgQ, &newMsg, 0 );
}

/*
 *  @function   receiveFromQ1
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - `reference to the struct holding the old data
 *  @return     None
 */
void receiveFromQ1(struct qData *oldData) {

    long long int *msg = NULL;

    xQueueReceive( msgQ, msg, portMAX_DELAY );

    if (msg == NULL) {
        oldData->success = false;
    }

    oldData->type = *msg & 0xff00;
    oldData->value = *msg & 0x00ff;

}
/*
void *queueThread(void *arg0) {

    bool qCreated;
    qData curData;

    createQ1();

    if (!qCreated) {
        1) Suspend all tasks
        2) Disable all interrupts
        3) Express error code
    }


    while (1) {

        // receiveFromQ1(&curData);

        // FSM stuff...

    }

    return (NULL);
}
*/

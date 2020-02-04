/*
 * sensor_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu
 */

#include "sensor_queue.h"

// xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );

// This queue will be MISO/FIFO.

bool createQ1() {

    msgQ = xQueueCreate( qLENGTH, qITEMSIZE);

    return msgQ == NULL ? false : true;
}

/*
   BaseType_t xQueueSendToBackFromISR( QueueHandle_t xQueue,
         const void * pvItemToQueue,
         TickType_t xTicksToWait );

    refer to page 110

    These two sending functions will be called from the callbacks,
    meaning they are outside the control of the OS.
 */

int sendTimeMsgToQ1(unsigned int timeVal) {

    long long int newMsg = TIME_DATA | timeVal;

    return xQueueSendToBackFromISR( msgQ, &newMsg, 0 );
}

int sendSensorMsgToQ1(int mmDist) {

    long long int newMsg = SENSOR_DATA | mmDist;

    return xQueueSendToBackFromISR( msgQ, &newMsg, 0 );
}

/*
   BaseType_t xQueueReceive( QueueHandle_t xQueue,
         void * const pvBuffer,
         TickType_t xTicksToWait );

    refer to page 113
    value to use --> portMAX_DELAY

    The receive however, is being called from other threads within
    the OS and thus will NOT have the FromISR.

    This function should be the one blocking.
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

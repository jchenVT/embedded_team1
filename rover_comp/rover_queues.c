/*
 * rover_queues.c
 *
 *  Created on: Feb 19, 2020
 *      Author: trieu
 */

#include <rover_uart.h>
#include "rover_queues.h"

static QueueHandle_t motorsQ = NULL;
static QueueHandle_t encoderQ = NULL;
static QueueHandle_t mqttReceiveQ = NULL;
static QueueHandle_t mqttSendQ = NULL;

/*
 *  @function   createMotorQ
 *              Wrapper for the RTOS function to create a queue. Uses
 *              the queue variables from rover_queues.h to define the
 *              amount of memory the queue will use.
 *
 *  @params     None
 *  @return     bool - indicates whether the message queue was created
 */
bool createMotorQ() {

    motorsQ = xQueueCreate( qLENGTH, mqITEMSIZE);
    /**************************/
    dbgOutputLoc(SQ_Q1_CREATE);
    /**************************/
    return motorsQ == NULL ? false : true;
}

bool createEncoderQ() {

    encoderQ = xQueueCreate( qLENGTH, eqITEMSIZE);

    return encoderQ == NULL ? false : true;
}

bool createMQTTReceiveQ() {

    encoderQ = xQueueCreate( qLENGTH, MRqITEMSIZE);

    return encoderQ == NULL ? false : true;
}

bool createMQTTSendQ() {

    mqttReceiveQ = xQueueCreate( qLENGTH, MSqITEMSIZE);

    return mqttReceiveQ == NULL ? false : true;
}

/*
 *  @function   sendMsgToMotors
 *              Wrapper for the RTOS function to send time data through
 *              the queue. Should be called from the callbacks (via ISR).
 *              Refer to page 110 of RTOS doc.
 *  @params
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendMsgToMotors(char address, char command, char speed) {

    struct motorData newMsg = {{address}, {command}, {speed}};
    /**************************/
    dbgOutputLoc(SQ_Q1_SEND_TIME);
    /**************************/
    return xQueueSendToBack( motorsQ, &newMsg, 0 );
}

/*
 *  @function   receiveFromQ1
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - `reference to the struct holding the old data
 *  @return     None
 */
/*
void receiveFromQ1(struct qData *oldData) {

    long long int msg = 0;

    dbgOutputLoc(SQ_Q1_RECEIVE);
    xQueueReceive( msgQ, &msg, portMAX_DELAY );

    if (&msg == NULL) {
        oldData->success = false;
    }

    oldData->type = msg & TYPE_MASK;
    oldData->value = msg & VALUE_MASK;

}
*/

bool receiveFromMotorsQ(struct motorData *oldData) {
    xQueueReceive( motorsQ, oldData, portMAX_DELAY );

    return oldData == NULL ? false : true;
}

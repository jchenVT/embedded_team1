/*
 * rover_queues.c
 *
 *  Created on: Feb 19, 2020
 *      Author: trieu
 */

#include <rover_queues.h>

static QueueHandle_t motorsQ = NULL;
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
    dbgOutputLoc(RQ_MotorQ_CREATE);
    /**************************/
    return motorsQ == NULL ? false : true;
}

bool createMQTTReceiveQ() {

    mqttReceiveQ = xQueueCreate( qLENGTH, MRqITEMSIZE);
    /**************************/
    dbgOutputLoc(RQ_MQTTReceive_CREATE);
    /**************************/
    return mqttReceiveQ == NULL ? false : true;
}

bool createMQTTSendQ() {

    mqttSendQ = xQueueCreate( qLENGTH, MSqITEMSIZE);
    /**************************/
    dbgOutputLoc(RQ_MQTTSend_CREATE);
    /**************************/
    return mqttSendQ == NULL ? false : true;
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
    dbgOutputLoc(RQ_MotorQ_SEND);
    /**************************/
    return xQueueSendToBack( motorsQ, &newMsg, 0 );
}

int sendMsgToReceiveQ(bool sensorType, long data, long data2) {
    struct receiveData newMsg = {sensorType, data, data2};

    return xQueueSendToBack( motorsQ, &newMsg, 0 );
}

int sendMsgToMQTTSendQ(int sendLoc, int data) {

    return 0;
}


bool receiveFromMotorsQ(struct motorData *oldData) {
    xQueueReceive( motorsQ, oldData, portMAX_DELAY );
    /**************************/
    dbgOutputLoc(RQ_MotorQ_RECEIVE);
    /**************************/
    return oldData == NULL ? false : true;
}

bool receiveFromMQTTReceiveQ(struct receiveData *oldData) {
    xQueueReceive( mqttReceiveQ, oldData, portMAX_DELAY );
    /**************************/
    dbgOutputLoc(RQ_MQTTReceive_RECEIVE);
    /**************************/
    return oldData == NULL ? false : true;
}

bool receiveFromMQTTSendQ() {

    /**************************/
    dbgOutputLoc(RQ_MQTTSend_RECEIVE);
    /**************************/
    return false;
}


/*
 * rover_queues.c
 *
 *  Created on: Feb 19, 2020
 *      Author: trieu
 */

#include <rover_queues.h>

static QueueHandle_t motorsQ = NULL;
static QueueHandle_t encoderQ = NULL;
static QueueHandle_t mqttReceiveQ = NULL;
static QueueHandle_t mqttSendQ = NULL;

/* Queue variables */
#define qLENGTH         48
#define mqITEMSIZE      3
#define eqITEMSIZE      1
#define MRqITEMSIZE     sizeof(struct receiveData)

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

bool createEncoderQ() {
    encoderQ = xQueueCreate( qLENGTH, eqITEMSIZE );
    /**************************/
    dbgOutputLoc(RQ_EncoderQ_CREATE);
    /**************************/
    return encoderQ == NULL ? false : true;
}

bool createMQTTReceiveQ() {

    mqttReceiveQ = xQueueCreate( qLENGTH, MRqITEMSIZE);
    /**************************/
    dbgOutputLoc(RQ_MQTTReceive_CREATE);
    /**************************/
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
int sendMsgToMotorsQ(char address, char command, char speed) {

    struct motorData newMsg = {address, command, speed};
    /**************************/
    dbgOutputLoc(RQ_MotorQ_SEND);
    /**************************/
    return xQueueSendToBack( motorsQ, &newMsg, 0 );
}

int sendMsgToEncoderQ() {
    bool newMsg = true;
    /**************************/
    dbgOutputLoc(RQ_EncoderQ_SEND);
    /**************************/
    return xQueueSendToBack( encoderQ, &newMsg, 0 );
}

int sendMsgToReceiveQ(bool sensorType, bool move, int angle_rotate, double data, double data2) {    struct receiveData newMsg = {sensorType, move, angle_rotate, data, data2};
    if (sensorType) {
        dbgOutputLoc(RQ_MQTTReceive_senSEND);
        return xQueueSendToBackFromISR( mqttReceiveQ, &newMsg, 0 );
    }
    else {
        dbgOutputLoc(RQ_MQTTReceive_spiSEND);
        return xQueueSendToBack( mqttReceiveQ, &newMsg, 0 );
    }
}

bool receiveFromMotorsQ(struct motorData *oldData) {
    xQueueReceive( motorsQ, oldData, portMAX_DELAY );
    /**************************/
    dbgOutputLoc(RQ_MotorQ_RECEIVE);
    /**************************/
    return oldData == NULL ? false : true;
}

bool receiveFromEncoderQ(bool *oldData) {
    xQueueReceive( encoderQ, oldData, portMAX_DELAY );
    /**************************/
    dbgOutputLoc(RQ_EncoderQ_RECEIVE);
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

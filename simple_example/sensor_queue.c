/*
 * sensor_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu, giffen
 */

#include "sensor_queue.h"

static QueueHandle_t sensorQ = NULL;

/*
 *  @function   createQs
 *              Wrapper for the RTOS function to create all the queues.
 *              Uses the queue variables from sensor_queue.h to define the
 *              amount of memory the queue will use.
 *
 *  @params     None
 *  @return     bool - indicates whether there was a successful setup
 */
bool setupQ() {

    sensorQ = xQueueCreate(qLENGTH, sizeof(struct qData));
    return ( sensorQ == NULL ? false : true );
}

/*
 *  @function   sendToProx1Q
 *              Wrapper to add to the proximity 1 sensor queue.
 *
 *  @params     timeVal - value from sensor
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendProxToSensorQ(int proxNum, int val) {

    struct qData newMsg;
    newMsg.sensorID = proxNum;
    newMsg.sensorVal = val;
    newMsg.r = 0;
    newMsg.g = 0;
    newMsg.b = 0;

    return xQueueSendToBackFromISR( sensorQ, &newMsg, 0 );
}

/*
 *  @function   sendToRGBQ
 *              Wrapper to add to the RGB sensor queue.
 *
 *  @params     value - value from RGB callback
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendRGBToSensorQ(int red, int green, int blue) {

    struct qData newMsg;
    newMsg.sensorID = RGB_DATA;
    newMsg.sensorVal = 0;
    newMsg.r = red;
    newMsg.g = green;
    newMsg.b = blue;

    return xQueueSendToBackFromISR( sensorQ, &newMsg, 0 );
}

/*
 *  @function   receiveFromProx1Q
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - reference to the struct holding the old data
 *  @return     None
 */
void receiveSensorQ(struct qData *oldData) {

    xQueueReceive( sensorQ, oldData, portMAX_DELAY );
}

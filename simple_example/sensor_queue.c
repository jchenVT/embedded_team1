/*
 * sensor_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu, giffen
 */

#include "sensor_queue.h"

static QueueHandle_t prox1Q = NULL;
static QueueHandle_t prox2Q = NULL;
static QueueHandle_t rgbQ = NULL;

/*
 *  @function   createQs
 *              Wrapper for the RTOS function to create all the queues.
 *              Uses the queue variables from sensor_queue.h to define the
 *              amount of memory the queue will use.
 *
 *  @params     None
 *  @return     bool - indicates whether there was a succesful setup
 */
bool setupQs() {

    prox1Q = xQueueCreate(qLENGTH, qITEMSIZE);
    prox2Q = xQueueCreate(qLENGTH, qITEMSIZE);
    rgbQ = xQueueCreate(qLENGTH, qITEMSIZE);
    return ( prox1Q == NULL ? false : true ) && ( prox2Q == NULL ? false : true ) && ( rgbQ == NULL ? false : true );
}

/*
 *  @function   sendToProx1Q
 *              Wrapper to add to the proximity 1 sensor queue.
 *
 *  @params     timeVal - value from sensor
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendToProx1Q(int value) {

    long long int newMsg = PROX1_DATA | value;
    return xQueueSendToBackFromISR( prox1Q, &newMsg, 0 );
}

/*
 *  @function   sendToProx1Q
 *              Wrapper to add to the proximity 2 sensor queue.
 *
 *  @params     value - value from sensor
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendToProx2Q(int value) {

    long long int newMsg = PROX2_DATA | value;
    return xQueueSendToBackFromISR( prox2Q, &newMsg, 0 );
}

/*
 *  @function   sendToRGBQ
 *              Wrapper to add to the RGB sensor queue.
 *
 *  @params     value - value from RGB callback
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendToRGBQ(int value) {

    long long int newMsg = RGB_DATA | value;
    return xQueueSendToBackFromISR( rgbQ, &newMsg, 0 );
}

/*
 *  @function   receiveFromProx1Q
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - reference to the struct holding the old data
 *  @return     None
 */
void receiveFromProx1Q(struct qData *oldData) {

    long long int msg = 0;
    xQueueReceive( prox1Q, &msg, portMAX_DELAY );

    oldData->type = msg & TYPE_MASK;
    oldData->value = msg & VALUE_MASK;
}

/*
 *  @function   receiveFromProx1Q
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - reference to the struct holding the old data
 *  @return     None
 */
void receiveFromProx2Q(struct qData *oldData) {

    long long int msg = 0;
    xQueueReceive( prox2Q, &msg, portMAX_DELAY );

    oldData->type = msg & TYPE_MASK;
    oldData->value = msg & VALUE_MASK;
}

/*
 *  @function   receiveFromProx1Q
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - reference to the struct holding the old data
 *  @return     None
 */
void receiveFromRGBQ(struct qData *oldData) {

    long long int msg = 0;
    xQueueReceive( rgbQ, &msg, portMAX_DELAY );

    oldData->type = msg & TYPE_MASK;
    oldData->value = msg & VALUE_MASK;
}

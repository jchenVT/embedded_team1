/*
 * mqtt_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu
 */

#include "mqtt_queue.h"

static QueueHandle_t pubQ = (void *)0;
static QueueHandle_t subArmQ = (void *)0;
static QueueHandle_t subArmSensorQ = (void *)0;
static QueueHandle_t subRoverQ = (void *)0;
static QueueHandle_t subRoverSensorQ = (void *)0;

/*
 *  @function   createMQTTQs
 *              Wrapper for the RTOS function to create the message queues
 *              for publishing and subscribing from a queue.
 *
 *  @params     None
 *  @return     bool - indicates whether the message queue was created
 */
bool createMQTTQs() {

    pubQ = xQueueCreate( qLENGTH, sizeof(struct qStringData));
    if (pubQ == (void *)0) {
        return false;
    }

    subArmQ = xQueueCreate( qLENGTH, sizeof(struct qArmMsg));
    if (subArmQ == (void *)0) {
        return false;
    }

    subArmSensorQ = xQueueCreate( qLENGTH, sizeof(struct qArmSensorMsg));
    if (subArmSensorQ == (void *)0) {
        return false;
    }

    subRoverQ = xQueueCreate( qLENGTH, sizeof(struct qRoverMsg));
    if (subRoverQ == (void *)0) {
        return false;
    }

    subRoverSensorQ = xQueueCreate( qLENGTH, sizeof(struct qRoverSensorMsg));
    if (subRoverSensorQ == (void *)0) {
        return false;
    }

    return true;
}

/*
 *  @function   sendToPubQ
 *              Wrapper for the RTOS function to send data through
 *              the queue. Holds data that needs to be published
 *              using the mqtt_client_app.c file.
 *
 *  @params     topic   - topic to be published from
 *              msg     - msg to be published to the topic
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendToPubQ(char * topic, char msg []) {

    struct qStringData data;
    data.topic = topic;
    strncpy(data.str, msg, MAXJSONSIZE);
    return xQueueSendToBackFromISR( pubQ, &data, 0 );
}

/*
 *  @function   sendToSub***Q
 *              Wrapper for the RTOS function to send data through
 *              the queue. Holds data that needs to be published
 *              using the mqtt_client_app.c file.
 *
 *  @params     topic   - topic to be published from
 *              msg     - msg to be published to the topic
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendToSubArmQ(struct qArmMsg msg) {
    return xQueueSendToBackFromISR( subArmQ, &msg, 0 );
}

int sendToSubArmSensorQ(struct qArmSensorMsg msg) {
    return xQueueSendToBackFromISR( subArmSensorQ, &msg, 0 );
}

int sendToSubRoverQ(struct qRoverMsg msg) {
    return xQueueSendToBackFromISR( subRoverQ, &msg, 0 );
}

int sendToSubRoverSensorQ(struct qRoverSensorMsg msg) {
    return xQueueSendToBackFromISR( subRoverSensorQ, &msg, 0 );
}

/*
 *  @function   sendToSubQ
 *              Wrapper for the RTOS function to send data through
 *              the queue. Holds data that was received from the
 *              mqtt callback.
 *
 *  @params     topic   - topic to be published from
 *              msg     - msg to be published to the topic
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int receiveFromPubQ(struct qStringData *oldData) {
    return xQueueReceive( pubQ, oldData, portMAX_DELAY );
}

/*
 *  @function   receiveFrom____Q
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - `reference to the struct holding the old data
 *  @return     None
 */
int receiveFromSubArmQ(struct qArmMsg *oldData) {
    return xQueueReceive( subArmQ, oldData, portMAX_DELAY );
}

int receiveFromSubArmSensorQ(struct qArmSensorMsg *oldData) {
    return xQueueReceive( subArmSensorQ, oldData, portMAX_DELAY );
}

int receiveFromSubRoverQ(struct qRoverMsg *oldData) {
    return xQueueReceive( subRoverQ, oldData, portMAX_DELAY );
}

int receiveFromSubRoverSensorQ(struct qRoverSensorMsg *oldData) {
    return xQueueReceive( subRoverSensorQ, oldData, portMAX_DELAY );
}

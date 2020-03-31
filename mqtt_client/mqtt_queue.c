/*
 * mqtt_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu
 */

#include "mqtt_queue.h"

static QueueHandle_t pubQ = NULL;
static QueueHandle_t subQ = NULL;

/*
 *  @function   createQs
 *              Wrapper for the RTOS function to create the message queues
 *              for publishing and subscribing from a queue.
 *
 *  @params     None
 *  @return     bool - indicates whether the message queue was created
 */
bool createQs() {

    pubQ = xQueueCreate( qLENGTH, qITEMSIZE );
    if (pubQ == NULL) {
        return false;
    }

    subQ = xQueueCreate( qLENGTH, qITEMSIZE );
    if (subQ == NULL) {
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
int sendToPubQ(int topic, long long int msg) {

    struct qData data = {topic, msg};
    return xQueueSendToBackFromISR( pubQ, &data, 0 );
}

/*
 *  @function   sendToSubQ
 *              Wrapper for the RTOS function to send data through
 *              the queue. Holds data that needs to be published
 *              using the mqtt_client_app.c file.
 *
 *  @params     topic   - topic to be published from
 *              msg     - msg to be published to the topic
 *  @return     pdPASS or errQUEUE_FULL - successfully added or not
 */
int sendToSubQ(int topic, long long int msg) {

    struct qData data = {topic, msg};
    return xQueueSendToBackFromISR( subQ, &data, 0 );
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
int receiveFromPubQ(int topic, long long int msg) {

    xQueueReceive( pubQ, &oldData, portMAX_DELAY );
}

/*
 *  @function   receiveFromQ1
 *              Wrapper for the RTOS function to data from the queue.
 *              This function will block. Refer to page 113 of RTOS doc.
 *
 *  @params     oldData - `reference to the struct holding the old data
 *  @return     None
 */
void receiveFromSubQ(struct qData *oldData) {

    xQueueReceive( subQ, &oldData, portMAX_DELAY );
}

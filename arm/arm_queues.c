/*
 * arm_queues.c
 *
 *  Created on: Feb 20, 2020
 *      Author: chris
 */

#include "arm_queues.h"

static QueueHandle_t movQ = NULL;

bool createMovQ() {

    movQ = xQueueCreate(movqLENGTH, movqITEMSIZE);

    return movQ == NULL ? false : true;
}

int sendMsgToMovQ(movqData_t newMsg) {
    movqData_t inMsg;

    inMsg.yee_value = newMsg.yee_value;
    inMsg.haw_value = newMsg.haw_value;
    inMsg.cow_value = newMsg.cow_value;
    inMsg.boy_value = newMsg.boy_value;

    return xQueueSendToBackFromISR(movQ, &inMsg, 0);
}

bool receiveFromMovQ(movqData_t *m) {

    xQueueReceive(movQ, m, portMAX_DELAY);

    return m != NULL;
}

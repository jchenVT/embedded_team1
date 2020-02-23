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

int sendMsgToMovQ(uint16_t yee, uint16_t haw, uint16_t cow, uint16_t boy) {
    movqData_t newMsg = {yee, haw, cow, boy};

    return xQueueSendToBackFromISR(movQ, &newMsg, 0);
}

bool receiveFromMovQ(movqData_t *m) {

    xQueueReceive(movQ, m, portMAX_DELAY);

    return m == NULL;
}

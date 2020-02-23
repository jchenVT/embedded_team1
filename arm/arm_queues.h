/*
 * arm_queues.h
 *
 *  Created on: Feb 20, 2020
 *      Author: chris
 */

#ifndef ARM_QUEUES_H_
#define ARM_QUEUES_H_

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

#define movqLENGTH      32
#define movqITEMSIZE    64

typedef struct movqData {
    uint16_t yee_value;
    uint16_t haw_value;
    uint16_t cow_value;
    uint16_t boy_value;
} movqData_t;

/* Routine declarations */
bool createMovQ();
int sendMsgToMovQ(uint16_t yee_value, uint16_t haw_value, uint16_t cow_value, uint16_t boy_value);
bool receiveFromMovQ(movqData_t *m);

#endif /* ARM_QUEUES_H_ */

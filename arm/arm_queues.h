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

#define ackQLENGTH      32
#define ackQITEMSIZE    8

typedef struct movqData {
    uint16_t yee_value; // x
    uint16_t haw_value; // y
    uint16_t cow_value; // z
    uint16_t boy_value; // claw
} movqData_t;

/* Routine declarations */
bool createMovQ();
int sendMsgToMovQ(movqData_t inMsg);
bool receiveFromMovQ(movqData_t *m);

bool createAckQ();
int sendMsgToAckQ(uint8_t ack);
bool receiveFromAckQ(uint8_t *ack);

#endif /* ARM_QUEUES_H_ */

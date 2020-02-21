/*
 * rover_queues.h
 *
 *  Created on: Feb 19, 2020
 *      Author: trieu
 */

#ifndef ROVER_QUEUES_H_
#define ROVER_QUEUES_H_

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

/* Motor Controller Data */
#define FORWARD_COMMAND 0b00000000
#define REVERSE_COMMAND 0b00000001
#define FR_WHEEL_ADDR   0b10000000
#define FL_WHEEL_ADDR   0b10000001
#define BM_WHEEL_ADDR   0b10000010

/* Queue variables */
#define qLENGTH         32
#define mqITEMSIZE      3
#define eqITEMSIZE      4 // CHANGE/CONFIRM
#define MRqITEMSIZE     4 // CHANGE/CONFIRM
#define MSqITEMSIZE     4 // CHANGE/CONFIRM

/* Struct used in RETURNING message from queue */
struct motorData {
    char address[1];
    char command[1];
    char speed[1];
};

/* Routine declarations */
bool createMotorQ();
bool createEncoderQ();
bool createMQTTReceiveQ();
bool createMQTTSendQ();
int sendMsgToMotors(char address, char command, char speed);
bool receiveFromMotorsQ(struct motorData *oldData);

#endif /* ROVER_QUEUES_H_ */


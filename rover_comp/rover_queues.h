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

#include <rover_debug.h>

/* Motor Controller Data */
#define FORWARD_COMMAND 0b00000000
#define REVERSE_COMMAND 0b00000001
#define FR_WHEEL_ADDR   0b10000000
#define FL_WHEEL_ADDR   0b10000001
#define BM_WHEEL_ADDR   0b10000010

/* Queue variables */
#define qLENGTH         32
#define mqITEMSIZE      3
#define eqITEMSIZE      1
#define MRqITEMSIZE     24 // CHANGE/CONFIRM
#define MSqITEMSIZE     4 // CHANGE/CONFIRM

/* Struct used in RETURNING message from queue */
struct motorData {
    char address[1];
    char command[1];
    char speed[1];
};

struct receiveData {
    bool type;
    long data;
    long data2;
};

/* Routine declarations */
bool createMotorQ();
bool createEncoderQ();
bool createMQTTReceiveQ();
bool createMQTTSendQ();

int sendMsgToMotorsQ(char address, char command, char speed);
int sendMsgToEncoderQ();
int sendMsgToReceiveQ(bool sensorType, long data, long data2);
int sendMsgToMQTTSendQ(int sendLoc, int data);

bool receiveFromMotorsQ(struct motorData *oldData);
bool receiveFromEncoderQ(bool *oldData);
bool receiveFromMQTTReceiveQ(struct receiveData *oldData);
bool receiveFromMQTTSendQ();

#endif /* ROVER_QUEUES_H_ */


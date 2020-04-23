/*
 * arm.h
 *
 *  Created on: Feb 20, 2020
 *      Author: chris
 */

#ifndef ARM_H_
#define ARM_H_

#include <FreeRTOS.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <pthread.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/timer.h>
#include <ti/drivers/UART.h>

#include "arm_queues.h"
#include "ti_drivers_config.h"

#include <timers.h>

/* Message queue wrapper for recv msgs */
#include "mqtt_queue.h"

/* JSON formatting used to push to msg queues */
#include "jsonFormat.h"

#include "uart_term.h"

#define STEPSIZE    10

#define CLAW_LOW    800
#define CLAW_HIGH   1800

#define X_LOW       380
#define X_HIGH      2430

#define Y_LOW       700
#define Y_HIGH      2300

#define Z_LOW       1000
#define Z_HIGH      2200

typedef enum debugState {WaitingForAngles, WaitingForAck} dState;
typedef enum armState {Moving, NotMoving} aState;

void *armDebugThread(void *arg0);
void *mainArmThread(void *arg0);
void *readMQTTThread(void *arg0);
void arm_init();
void movementCallback();
void timerCallback(TimerHandle_t xTimer);

#endif /* ARM_H_ */

/*
 * star.h
 *
 *  Created on: Mar 31, 2020
 *      Author: giffen
 */

#ifndef STAR_H_
#define STAR_H_

#define receiveLidarSensor Random_getNumber
#define starrr Random_seedAutomatic

/* Timer */
#include <ti/drivers/Timer.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <ti/drivers/utils/Random.h>

/* Message queue wrapper for recv msgs */
#include "mqtt_queue.h"

/* JSON formatting used to push to msg queues */
#include "jsonFormat.h"

#include "uart_term.h"

void *starThread(void *arg0);
void timerCallback(TimerHandle_t xTimer);
void timerCallbackDebug(TimerHandle_t xTimer);

#endif /* STAR_H_ */

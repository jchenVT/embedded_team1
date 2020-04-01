/*
 * star.h
 *
 *  Created on: Mar 31, 2020
 *      Author: giffen
 */

#ifndef STAR_H_
#define STAR_H_

/* Timer */
#include <ti/drivers/Timer.h>
#include <FreeRTOS.h>
#include <timers.h>

/* Message queue wrapper for recv msgs */
#include "mqtt_queue.h"

/* JSON formatting used to push to msg queues */
#include "jsonFormat.h"

#define SUB_TOPIC   (enum topicVal) ARM_SENSORS
#define PUB_TOPIC   (enum topicVal) ARM

void *starThread(void *arg0);
void timerCallback(TimerHandle_t xTimer);

#endif /* STAR_H_ */

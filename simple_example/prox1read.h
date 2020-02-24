/*
 *  prox1read.h
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#ifndef PROX1READ_H_
#define PROX1READ_H_

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include <FreeRTOS.h>
#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/timer.h>
#include <timers.h>

/* Queue files */
#include "sensor_queue.h"

/* Debug files */
#include "debug.h"


/* Main Thread */
void *readProximity1Thread(void *arg0);
void timerCallback(TimerHandle_t xTimer);

#endif /* PROX1READ_H_ */

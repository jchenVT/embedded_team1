/*
 *  proxread.h
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#ifndef PROXREAD_H_
#define PROXREAD_H_

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
#include "uart_term.h"


/* Main Thread */
void *readProximityThread(void *arg0);
void timerCallback(TimerHandle_t xTimer);

#endif /* PROXREAD_H_ */

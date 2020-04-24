/*
 *  sensor_read.h
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#ifndef SENSOR_READ_H_
#define SENSOR_READ_H_

#include <math.h>

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include <FreeRTOS.h>
#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/timer.h>
#include <timers.h>

/* Queue files */
#include "sensor_queue.h"
#include "mqtt_queue.h"

/* Debug files */
#include "uart_term.h"

/* Slave attributes */
#define SENSORS 0
#define OPT_ADDR 0x29 // see TCS34725 documentation pg. 3

/* States for the timer */
enum State {OFF, ON, PAUSE};

/* Main Thread */
void *readSensorsThread(void *arg0);
void timerStartCallback(TimerHandle_t xTimer);
void timerProxCallback(TimerHandle_t xTimer);
void timerRGBCallback(TimerHandle_t xTimer);

#endif /* SENSOR_READ_H_ */

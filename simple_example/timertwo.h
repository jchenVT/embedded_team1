/*
 * timertwo.h
 *
 *  Created on: Feb 4, 2020
 *      Author: chris
 */

#ifndef TIMERTWO_H_
#define TIMERTWO_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

/* POSIX Header files */
#include <pthread.h>

#include "ti_drivers_config.h"

/* Driver Header files */
#include <ti/drivers/ADC.h>

#include "sensor_queue.h"

#define TIMER_PERIOD 75000
#define VCONVERSION 1000
#define MMCONVERSION1 538.48
#define MMCONVERSION2 -1.172

void timer75Callback();
int convertToMM(uint32_t mV);

#endif /* TIMERTWO_H_ */

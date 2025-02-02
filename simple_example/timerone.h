/*
 * timerone.h
 *
 *  Created on: Feb 3, 2020
 *      Author: chris
 */

#ifndef TIMERONE_H_
#define TIMERONE_H_

#include <stdio.h>
#include <stdlib.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

#include "ti_drivers_config.h"
#include "sensor_queue.h"
#include "debug.h"

void timerSecondCallback(Timer_Handle myHandle);

#endif /* TIMERONE_H_ */

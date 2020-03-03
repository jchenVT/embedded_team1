/*
 *  rgbread.h
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#ifndef RGBREAD_H_
#define RGBREAD_H_

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include <FreeRTOS.h>
#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Queue files */
#include "sensor_queue.h"

/* Debug files */
#include "debug.h"

/* Slave attributes */
#define SENSORS 0
#define OPT_ADDR 0x29 // see TCS34725 documentation pg. 3

/* Main Thread */
void *readRGBThread(void *arg0);
//void i2cCallback(I2C_Handle handle, I2C_Transaction *msg, bool status);

#endif /* RGBREAD_H_ */

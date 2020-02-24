/*
 * timerone.h
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#ifndef ROVER_SPI_H_
#define ROVER_SPI_H_

/* C-Libraries */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Project Files */
#include <rover_debug.h>
#include <rover_queues.h>
#include <rover_uart.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include "ti_drivers_config.h"

#include <FreeRTOS.h>
#include <timers.h>

#define THREADSTACKSIZE 1024
#define SPI_MSG_LENGTH  5
#define MAX_LOOP        10
#define TIMER_LENGTH    1000

enum encoders {e128, e129, e130};

void spi_setup();
void timer_setup();
void spi_close();

void timerCallback(TimerHandle_t myHandle);
void readEncoder(int encoder);
bool transferData(int encoder);

#endif /* ROVER_SPI_H_ */

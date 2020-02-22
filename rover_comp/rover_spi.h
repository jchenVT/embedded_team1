/*
 * timerone.h
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#ifndef ROVER_SPI_H_
#define ROVER_SPI_H_

/* POSIX Header files */
#include <semaphore.h>
#include <unistd.h>

/* C-Libraries */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Project Files */
#include <rover_queues.h>
#include <rover_uart.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include "ti_drivers_config.h"

#define THREADSTACKSIZE (1024)
#define SPI_MSG_LENGTH  (30)
#define MASTER_MSG      ("Hello from master, msg#: ")
#define MAX_LOOP        (10)

void slaveReadyFxn(uint_least8_t index);
void spi_setup();
void spi_close();

void *mainSPIThread(void *arg0);


#endif /* ROVER_SPI_H_ */

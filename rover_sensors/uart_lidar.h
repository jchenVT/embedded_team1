#ifndef _UART_LIDAR_
#define _UART_LIDAR_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stdbool.h>
#include <stdio.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#include "debug.h"
#include "queues.h"


static UART_Handle uart_lidar;
static UART_Params uart_lidar_params;


void uart_lidar_init();

// TODO:	see if this needs to be callback or blocking read
// 			parse uart messages from lidar somehow

void * uart_lidar_callback(UART_Handle, void * buf, size_t count);

void * uartLidarThread(void * arg0);

#endif

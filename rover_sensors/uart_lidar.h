#ifndef _UART_LIDAR_
#define _UART_LIDAR_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stdbool.h>
#include <stdio.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>
#include "ti_drivers_config.h"

#include "uart_term.h"
#include "debug.h"
#include "queues.h"

#define UART_BUF_SIZE 100

typedef struct point {
	uint16_t angle_raw;
	uint16_t distance_raw;
	double angle;
	double distance;
} point_t;

extern UART_Handle uart_lidar;

void uart_lidar_init();

// TODO:	see if this needs to be callback or blocking read
// 			parse uart messages from lidar somehow

void uart_timer_init();

void uart_lidar_callback(UART_Handle handle, void * buf, size_t count);

void timer_lidar_callback(Timer_Handle handle);

void send_point_uart_debug(point_t point);

void * uartLidarThread(void * arg0);


#endif

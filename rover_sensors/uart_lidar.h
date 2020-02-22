#ifndef _UART_LIDAR_
#define _UART_LIDAR_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdbool.h>
#include <stdio.h>
#include "ti_drivers_config.h"


static UART_Handle uart_lidar;
static UART_Params uart_lidar_params;
static QueueHandle_t uart_lidar_q;

struct uart_message {
	uint8_t array_len;
	unsigned char msg[50];	
};

void uart_lidar_init();

void * uart_lidar_callback(UART_Handle, void * buf, size_t count);

void * uartLidarThread(void * arg0);

#endif

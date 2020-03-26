#ifndef _ROVER_SENSORS_QUEUES_
#define _ROVER_SENSORS_QUEUES_

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "ti_drivers_config.h"

#include <stdbool.h>
#include <stdio.h>

#include "debug.h"

typedef struct uart_message {
	uint8_t array_len;
	char msg [100];	
} uart_message_t;

extern QueueHandle_t uart_debug_q;
extern QueueHandle_t lidar_data_q;
extern QueueHandle_t spi_start_q;

void queues_init();

#endif

#ifndef _ROVER_SENSORS_QUEUES_
#define _ROVER_SENSORS_QUEUES_

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "ti_drivers_config.h"

#include <stdbool.h>
#include <stdio.h>

#include "debug.h"


extern QueueHandle_t lidar_data_q;
extern QueueHandle_t spi_start_q;

void queues_init();

#endif

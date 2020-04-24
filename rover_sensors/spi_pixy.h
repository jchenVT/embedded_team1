#ifndef _SPI_PIXY_H
#define _SPI_PIXY_H

#include <FreeRTOS.h>
#include <task.h>

#include <stdlib.h> 
#include <stdbool.h>

#include <ti/drivers/I2C.h>
#include <ti/drivers/Timer.h>
#include "ti_drivers_config.h"

#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "queues.h"

#define TX_MSGSIZE 6


typedef struct block {
	uint16_t signature;
	uint16_t x_center;
	uint16_t y_center;
	uint16_t width;
	uint16_t height;
	uint16_t angle;
	uint8_t tracking_index;
	uint8_t age;
} Block_t;


//void spi_pixy_init();
I2C_Handle spi_pixy_init();

void * spiThread(void *arg0);

// void spi_pixy_callback( I2C_Handle handle, I2C_Transaction *transaction, bool status);

void send_pixy_ccc_spi();

bool checksum_check_ccc( uint8_t byte_arr[] );

Block_t convert_to_block_t( uint8_t block_array[] );


void timer_spi_callback( Timer_Handle timer_handle );


#endif 

#ifndef _SPI_PIXY_H
#define _SPI_PIXY_H

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "debug.h"
#include <ti/drivers/SPI.h>
#include <stdbool.h>

#include "ti_drivers_config.h"

#define MSGSIZE 60

static SPI_Handle spi;
static SPI_Params spi_params;

static const uint8_t request_packet_ccc [] = {0xc1, 0xae, 32, 2, 0xFF, 0x03};
static uint8_t recv_packet_ccc[60];

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


void spi_pixy_init();

void spi_pixy_callback( SPI_Handle handle, SPI_Transaction *transaction );

void send_pixy_ccc_spi();

bool checksum_check_ccc( uint8_t byte_arr[] );

Block_t convert_to_block_t( uint8_t block_array[] );

// TODO Include the timer for running this here

void timer_spi_callback();


#endif 

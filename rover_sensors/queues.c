#include "queues.h"

QueueHandle_t lidar_data_q = NULL;
QueueHandle_t spi_start_q = NULL;

void queues_init()
{

    spi_start_q = xQueueCreate( 1, sizeof(char));
    if (spi_start_q == NULL) 
        stop_all(FAIL_UART_INIT);

    // lidar_data_q = xQueueCreate();
     
}

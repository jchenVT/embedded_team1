#include "queues.h"

QueueHandle_t uart_debug_q = NULL;
QueueHandle_t lidar_data_q = NULL;
QueueHandle_t spi_start_q = NULL;

void queues_init()
{
    // creating uart queue
    /**********************/
    dbgOutputLoc(INIT_UART_DEBUG_Q);
    /**********************/
    uart_debug_q = xQueueCreate( 64, sizeof(uart_message_t));
    if (uart_debug_q == NULL) 
        stop_all(FAIL_UART_INIT);

    spi_start_q = xQueueCreate( 10, sizeof(char));
    if (spi_start_q == NULL) 
        stop_all(FAIL_UART_INIT);

    // lidar_data_q = xQueueCreate();
     
}

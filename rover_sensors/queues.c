#include "queues.h"

QueueHandle_t uart_debug_q = NULL;

void queues_init()
{
    // creating uart queue
    /**********************/
    dbgOutputLoc(INIT_UART_DEBUG_Q);
    /**********************/
    uart_debug_q = xQueueCreate( 64, 51);
    if (uart_debug_q == NULL) 
        stop_all(FAIL_UART_INIT);
     
}

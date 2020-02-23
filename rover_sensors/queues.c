#include "queues.h"


void queues_init()
{
    // creating uart queue
    uart_debug_q = xQueueCreate( 64, 51);
    if (uart_debug_q == NULL) 
        stop_all(FAIL_UART_INIT);
     
}

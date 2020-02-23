#include "uart_debug.h"

void uart_debug_init()
{
    UART_Params_init(&uart_debug_params);
    uart_debug_params.writeDataMode = UART_DATA_TEXT;
    uart_debug_params.readDataMode = UART_DATA_TEXT;
    uart_debug_params.readReturnMode = UART_RETURN_FULL;
    uart_debug_params.readEcho = UART_ECHO_OFF;
    uart_debug_params.baudRate = 9600;

    /*****************************/
    dbgOutputLoc(UART_OPENING);
    /*****************************/

    uart_debug = UART_open(CONFIG_UART_0, &uart_debug_params);
    if (uart_debug == NULL)
        stop_all(FAIL_UART_INIT);

}


void *uartDebugThread(void *arg0)
{
    uart_message_t msg;

    while(1)
    {

        xQueueReceive(uart_debug_q, &msg, portMAX_DELAY);

        dbgOutputLoc(UART_WRITING); 

        UART_write(uart_debug, msg.msg, msg.array_len);
    }
}

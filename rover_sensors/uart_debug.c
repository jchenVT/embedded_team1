#include "uart_debug.h"

UART_Handle uart_debug = NULL;
UART_Params uart_debug_params;

void uart_debug_init()
{
    UART_Params_init(&uart_debug_params);
    uart_debug_params.writeDataMode = UART_DATA_TEXT;
    uart_debug_params.writeMode = UART_MODE_BLOCKING;
    uart_debug_params.readEcho = UART_ECHO_OFF;
    uart_debug_params.baudRate = 9600;
    /*****************************/
    dbgOutputLoc(UART_DEBUG_OPEN);
    /*****************************/
    uart_debug = UART_open(CONFIG_UART_0, &uart_debug_params);
    if (uart_debug == NULL)
    {
        stop_all(FAIL_UART_INIT);
    }

    /*****************************/
    dbgOutputLoc(UART_DEBUG_INITIALIZED);
    /*****************************/

    /*****************************
    uart_message_t test;
    const unsigned char test_str[10] = "a test";
    strncpy(test.msg, test_str, 6);
    test.array_len = 6;
    dbgOutputLoc(UART_DEBUG_TEST);
    UART_write(uart_debug, test.msg, test.array_len);
    dbgOutputLoc(UART_DEBUG_TEST_DONE);
    *****************************/

}


void *uartDebugThread(void *arg0)
{
    uart_message_t msg;

    while(1)
    {

        xQueueReceive(uart_debug_q, &msg, portMAX_DELAY);

        dbgOutputLoc(UART_DEBUG_WRITE); 

        UART_write(uart_debug, msg.msg, msg.array_len);
    }
}

#include "uart_lidar.h"

UART_Handle uart_lidar = NULL;

void uart_lidar_init()
{
    // TODO: Replace these with proper UART connection to lidar
    //
    UART_Params_init(&uart_lidar_params);
    uart_lidar_params.writeMode = UART_MODE_BLOCKING;
    uart_lidar_params.readMode = UART_MODE_CALLBACK;
    uart_lidar_params.writeDataMode = UART_DATA_BINARY;
    uart_lidar_params.readDataMode = UART_DATA_BINARY;
    uart_lidar_params.readReturnMode = UART_RETURN_FULL;
    uart_lidar_params.readEcho = UART_ECHO_OFF;
    uart_lidar_params.baudRate = 105200;
    /*****************************/
    dbgOutputLoc(UART_LIDAR_OPEN);
    /*****************************/
    uart_lidar = UART_open(CONFIG_UART_1, &uart_lidar_params); 
    if (uart_lidar == NULL)
        stop_all(FAIL_UART_INIT);
    // send scan mode to start the lidar
    // unsigned char start_msg[2] = {0xA5, 0x20};
    // UART_write(uart_lidar, start_msg, 2);

}

void *uartLidarThread(void *arg0)
{
    return NULL;
    /*
    while(1)
    {

        xQueueReceive( uart_lidar_q, &msg, portMAX_DELAY );

        UART_write(uart_lidar, msg.msg, msg.array_len);
    }
    */
}

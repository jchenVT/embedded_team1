#include "uart_lidar.h"
#include "debug.h"

void uart_lidar_init()
{
    // TODO: Replace these with proper UART connection to lidar
    UART_Params_init(&uart_lidar_params);
    uart_lidar_params.writeMode = UART_MODE_BLOCKING;
    uart_lidar_params.readMode = UART_MODE_CALLBACK;
    uart_lidar_params.writeDataMode = UART_DATA_BINARY;
    uart_lidar_params.readDataMode = UART_DATA_BINARY;
    uart_lidar_params.readReturnMode = UART_RETURN_FULL;
    uart_lidar_params.readEcho = UART_ECHO_OFF;
    uart_lidar_params.baudRate = 9600;

    uart_lidar = UART_open(CONFIG_UART1, &uart_lidar_params); 

    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

    // send scan mode to start the lidar
    unsigned char start_msg[2] = {0xA5, 0x20};
    UART_write(uart_lidar, start_msg, 2);

    // initialize uart queue
    // TODO: this probably needs to be changed to queue that receives uart queu
    uart_lidar_q = xQueueCreate( 64, 51);
    if (uart_lidar_q == NULL) 
        stop_all(FAIL_UART_INIT);
}

void *uartLidarThread(void *arg0)
{
    struct uart_message msg;

    while(1)
    {

        xQueueReceive( uart_lidar_q, &msg, portMAX_DELAY );


        UART_write(uart_lidar, msg.msg, msg.array_len);
    }
}

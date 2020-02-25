#include "uart_lidar.h"

UART_Handle uart_lidar = NULL;
static uint8_t uart_buf[UART_BUF_SIZE];

static Timer_Handle timer_lidar = NULL;

void uart_lidar_init()
{
    // TODO: Replace these with proper UART connection to lidar
    //
    UART_Params uart_lidar_params;
    UART_Params_init(&uart_lidar_params);
    uart_lidar_params.writeMode = UART_MODE_BLOCKING;
    uart_lidar_params.readMode = UART_MODE_CALLBACK;
    uart_lidar_params.readCallback = uart_lidar_callback;
    uart_lidar_params.writeDataMode = UART_DATA_BINARY;
    uart_lidar_params.readDataMode = UART_DATA_BINARY;
    uart_lidar_params.readReturnMode = UART_RETURN_FULL;
    uart_lidar_params.readEcho = UART_ECHO_OFF;
    uart_lidar_params.baudRate = 115200;
    /*****************************/
    dbgOutputLoc(UART_LIDAR_OPEN);
    /*****************************/
    uart_lidar = UART_open(CONFIG_UART_1, &uart_lidar_params); 
    if (uart_lidar == NULL)
        stop_all(FAIL_UART_INIT);
    /*****************************/
    dbgOutputLoc(UART_LIDAR_OPEN_DONE);
    /*****************************/
}

void uart_timer_init()
{
    Timer_Params timer_lidar_params;
    Timer_Params_init(&timer_lidar_params);
    timer_lidar_params.periodUnits = Timer_PERIOD_HZ;
    timer_lidar_params.period = 1;
    timer_lidar_params.timerMode = Timer_CONTINUOUS_CALLBACK;
    timer_lidar_params.timerCallback = timer_lidar_callback;
    timer_lidar = Timer_open(CONFIG_TIMER_0, &timer_lidar_params);
    if (timer_lidar == NULL)
        stop_all(FAIL_SPI_TIMER_INIT);
    if (Timer_start(timer_lidar) == Timer_STATUS_ERROR)
        stop_all(FAIL_SPI_TIMER_INIT);

}

void uart_lidar_callback(UART_Handle handle, void * buf, size_t count)
{
    uint8_t * cast_buf;  
    cast_buf = buf;
    uart_message_t msg;

}

void timer_lidar_callback(Timer_Handle handle)
{
    UART_read(uart_lidar, uart_buf, UART_BUF_SIZE);
}

void *uartLidarThread(void *arg0)
{
    // Open Timer
    uart_timer_init();

    // send scan mode to start the lidar
    // in debug mode rn
    unsigned char start_msg[2] = {0xA5, 0x21};
    // unsigned char start_msg[6] = {0xA5, 0xA8, 0x02, 0x00, 0x00, 0xC};
    UART_write(uart_lidar, start_msg, 2);

    uart_message_t msg;

    while(1)
    {
        
        xQueueReceive( lidar_data_q, &msg, portMAX_DELAY );

    }
}

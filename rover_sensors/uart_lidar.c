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
    uart_lidar_params.readEcho = UART_ECHO_OFF;
    uart_lidar_params.writeMode = UART_MODE_BLOCKING;
    uart_lidar_params.readMode = UART_MODE_BLOCKING;
    uart_lidar_params.readTimeout = UART_WAIT_FOREVER;
    uart_lidar_params.writeTimeout = UART_WAIT_FOREVER;
    uart_lidar_params.writeDataMode = UART_DATA_BINARY;
    uart_lidar_params.readDataMode = UART_DATA_BINARY;
    uart_lidar_params.readReturnMode = UART_RETURN_FULL;
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

/*
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
*/

void send_point_uart_debug(point_t point)
{
    uart_message_t msg;
    msg.array_len = snprintf(msg.msg, 100, "lidar point=d:%fd,a:%fd", point.distance, point.angle);
    xQueueSend(uart_debug_q, &msg, portMAX_DELAY);
}

void *uartLidarThread(void *arg0)
{

    uart_message_t msg;


    const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
    // Open Timer
    // uart_timer_init();
    
    // Check lidar health
    
    /*
    const unsigned char get_health_msg[2] = {0xA5, 0x52};
    UART_write(uart_lidar, get_health_msg, 2);
    UART_read(uart_lidar, uart_buf, 5);

    msg.array_len = snprintf(msg.msg, 100, "\nuart health read");
    xQueueSend(uart_debug_q, &msg, portMAX_DELAY);

    uint8_t health_status = uart_buf[3];
    if (health_status != 0)
        stop_all(UART_LIDAR_HEALTH_FAIL); 

    
    msg.array_len = snprintf(msg.msg, 100, "\nuart health good");
    xQueueSend(uart_debug_q, &msg, portMAX_DELAY);

    */ 
    
    /*****************************/
    dbgOutputLoc(UART_LIDAR_HEALTH);
    /*****************************/


    // send scan mode to start the lidar
    unsigned char start_msg[2] = {0xA5, 0x20};
    // unsigned char start_msg[6] = {0xA5, 0xA8, 0x02, 0x00, 0x00, 0xC};
    UART_write(uart_lidar, start_msg, 2);
    // UART_read(uart_lidar, uart_buf, 5);

    msg.array_len = snprintf(msg.msg, 100, "\nsent start ");
    xQueueSend(uart_debug_q, &msg, portMAX_DELAY);

    vTaskDelay ( xDelay);

    /*****************************/
    dbgOutputLoc(UART_LIDAR_STARTED);
    /*****************************/

    while(1)
    {
        UART_read(uart_lidar, uart_buf, 15);
        
        msg.array_len = snprintf(msg.msg, 100, "\nread message");
        xQueueSend(uart_debug_q, &msg, portMAX_DELAY);

        size_t i = 0;
        while(i<UART_BUF_SIZE-1)
        {
            if(uart_buf[i] == 0xA5 && uart_buf[i+1] == 0x5A)
            {
                msg.array_len = snprintf(msg.msg, 100, "\n found message start");
                xQueueSend(uart_debug_q, &msg, portMAX_DELAY);
                i += 2;
                break;
            }
            i++;
        }

        uint8_t quality;
        quality = ((uint8_t) uart_buf[0]) >> 2; 
        if (quality == 0)  
            continue; 
        point_t point;
        point.angle_raw =  (uint8_t) uart_buf[2];
        point.angle_raw <<= 8;
        point.angle_raw |= ((uint8_t) uart_buf[1] >> 1);

        point.distance_raw = (uint8_t) uart_buf[4];
        point.distance_raw <<= 8;
        point.angle_raw |= (uint8_t) uart_buf[3];
        
        point.distance = point.distance_raw / 4.0;
        point.angle = point.angle_raw / 64.0;

        /*****************************/
        dbgOutputLoc(UART_LIDAR_POINT_IN);
        /*****************************/

        send_point_uart_debug(point);
    }
}

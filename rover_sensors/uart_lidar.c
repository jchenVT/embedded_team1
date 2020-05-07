#include "uart_lidar.h"

#define len 50
UART_Handle uart_lidar = NULL;
unsigned char uart_buf[UART_BUF_SIZE];
char str[len];

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

    //8n1
    uart_lidar_params.dataLength = UART_LEN_8;
    uart_lidar_params.parityType = UART_PAR_NONE;
    uart_lidar_params.stopBits = UART_STOP_ONE;


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
    snprintf(str, len, "\nlidar point=d:%fd,a:%fd", point.distance, point.angle);
    UART_PRINT(str);
}

void *uartLidarThread(void *arg0)
{

    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
    // Open Timer
    // uart_timer_init();
    
    const unsigned char reset_msg[4] = {0xA5, 0x40, 0x00, 0x00 ^ 0xA5 ^ 0x40 ^ 0x00};
    UART_write(uart_lidar, reset_msg, 4);

    vTaskDelay ( xDelay);

    // Check lidar health
    
    const unsigned char health_msg[4] = {0xA5, 0x52, 0x00, 0x00 ^ 0xA5 ^ 0x52 ^ 0x00};
    UART_write(uart_lidar, health_msg, 4);

    snprintf(str, len, "\nuart health request");
    UART_PRINT(str);

    vTaskDelay ( xDelay);

    UART_readPolling(uart_lidar, uart_buf, 7);
    snprintf(str, len, "\nuart health read");

    uint8_t health_status = uart_buf[3];

    if (health_status == 0 && uart_buf[0] == 0xA5 && uart_buf[1] == 0x5A && uart_buf[6] == 0x06)
        snprintf(str, len, "\nuart health good");
    else 
    {
        snprintf(str, 50, "\nuart health:  %ux, %ux, %ux, %ux, %ux, %ux, %ux", uart_buf[0], uart_buf[1], uart_buf[2], uart_buf[3], uart_buf[4],  uart_buf[5], uart_buf[6]);
    }

    UART_PRINT(str);

    vTaskDelay ( xDelay);

    // send scan mode to start the lidar
    unsigned char start_msg[4] = {0xA5, 20, 0x00, 0x00 ^ 0xA5 ^ 0x52 ^ 0x00};
    UART_write(uart_lidar, start_msg, 4);

    UART_read(uart_lidar, uart_buf, 7);
    
    if(1) //  uart_buf[0] == 0xA5 && uart_buf[1] == 0x5A && uart_buf[2] == 0x05 && uart_buf[6] == 0x81)
    {
        // msg.array_len = snprintf(msg.msg, 100, "\nresponse:     %X, %X, %x, %x, %x, %x, %x", uart_buf[0], uart_buf[1], uart_buf[2], uart_buf[3], uart_buf[4],  uart_buf[5], uart_buf[6]);
        // xQueueSend(uart_debug_q, &msg, portMAX_DELAY);
    }

    /*****************************/
    dbgOutputLoc(UART_LIDAR_STARTED);
    /*****************************/

    while(1)
    {
        //msg.array_len = snprintf(msg.msg, 100, "\nreading");
        // xQueueSend(uart_debug_q, &msg, portMAX_DELAY);

        UART_read(uart_lidar, uart_buf, 5);
        
        uint8_t quality, s, not_s, complement ;
        quality = ((uint8_t) uart_buf[0]) >> 2; 
        s = uart_buf[0] & 1;
        not_s = uart_buf[0] & 2;
        complement = uart_buf[1] & 1;
            
        if (quality == 0 && s != not_s && complement == 1)  
        {
            // msg.array_len = snprintf(msg.msg, 100, "\nbad quality or wrong message");
            // xQueueSend(uart_debug_q, &msg, portMAX_DELAY);
            continue; 
        }
        point_t point;
        point.angle_raw =  (uint8_t) uart_buf[2];
        point.angle_raw <<= 7;
        point.angle_raw |= ((uint8_t) uart_buf[1] >> 1);

        point.distance_raw = (uint8_t) uart_buf[4];
        point.distance_raw <<= 8;
        point.distance_raw |= (uint8_t) uart_buf[3];
        
        point.distance = point.distance_raw / 4.0;
        point.angle = point.angle_raw / 64.0;

        /*****************************/
        dbgOutputLoc(UART_LIDAR_POINT_IN);
        /*****************************/

        send_point_uart_debug(point);
    }
}

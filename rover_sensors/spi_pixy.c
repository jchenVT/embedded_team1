#include "spi_pixy.h"

static I2C_Handle i2c = NULL;
static Timer_Handle timer_pixy = NULL;
static const uint8_t request_packet_ccc [] = {0xc1, 0xae, 32, 2, 0xFF, 0x03};
static const uint8_t request_packet_version [] = {0xc1, 0xae, 14, 0};
// static void * request_packet_ccc = malloc(sizeof(uint8_t) * 6);

static uint8_t recv_packet_ccc[60];

void spi_pixy_init()
{
    I2C_Params i2c_params;
    I2C_Params_init(&i2c_params);
    i2c_params.transferMode = I2C_MODE_CALLBACK;
    i2c_params.transferCallbackFxn = i2c_pixy_callback;
    i2c_params.bitRate = I2C_400kHz;
    /*****************************/
    dbgOutputLoc(SPI_SPI_OPEN);
    /*****************************/
    i2c = I2C_open(CONFIG_I2C_0, &i2c_params);
    if (i2c == NULL)
        stop_all(FAIL_SPI_INIT);

    Timer_Params timer_pixy_params;
    Timer_Params_init(&timer_pixy_params);
    timer_pixy_params.periodUnits = Timer_PERIOD_HZ;
    timer_pixy_params.period = 1;
    timer_pixy_params.timerMode = Timer_CONTINUOUS_CALLBACK;
    timer_pixy_params.timerCallback = timer_spi_callback;
    
    /*****************************/
    dbgOutputLoc(SPI_TIMER_OPEN);
    /*****************************/
    timer_pixy = Timer_open(CONFIG_TIMER_0, &timer_pixy_params);
    if (timer_pixy == NULL)
        stop_all(FAIL_SPI_TIMER_INIT);
    if (Timer_start(timer_pixy) == Timer_STATUS_ERROR)
        stop_all(FAIL_SPI_TIMER_INIT);
            
}


void i2c_pixy_callback(I2C_Handle handle, I2C_Transaction *transaction, bool status)
{
    
    /*****************************/
    dbgOutputLoc(SPI_PIXY_CALLBACK);
    /*****************************/


    // Check if transaction went through fine
    if (status == false)
    {
        uart_message_t uart_msg;
        uart_msg.array_len = snprintf(uart_msg.msg, 100, "fako\n");
        xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
        return;
    }
    else
    {
        uart_message_t uart_msg;
        uart_msg.array_len = snprintf(uart_msg.msg, 100, "not fako?\n");
        xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
        return;
    }

    /*
    Block_t block;
    uart_message_t uart_msg;
    block = convert_to_block_t(recv_packet_ccc);
    
    uart_msg.array_len = snprintf(uart_msg.msg, 100, "pixy,sig:%u,x:%u,y:%u,w:%u,h:%u",
             block.signature, block.x_center, block.y_center,
             block.width, block.height);
    
    xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    */
}

void timer_spi_callback(Timer_Handle timer_handle)
{
    /*****************************/
    dbgOutputLoc(SPI_TIMER_CALLBACK);
    /*****************************/
    
    send_pixy_ccc_spi();
}


void send_pixy_ccc_spi()
{
    /*****************************/
    dbgOutputLoc(SPI_SEND_TRANSACTION);
    /*****************************/
    I2C_Transaction i2c_transaction;
    i2c_transaction.slaveAddress = 0x69;
    i2c_transaction.writeBuf = (void *)request_packet_ccc;
    i2c_transaction.writeCount = 6;
    i2c_transaction.readBuf = (void *)recv_packet_ccc;
    i2c_transaction.readCount = 20;
    if (i2c == NULL)
        stop_all(FAIL_SPI_TRANSACTION);

    I2C_transfer(i2c, &i2c_transaction);
}

Block_t convert_to_block_t(uint8_t block_array[])
{
    Block_t block;
    block.signature =   block_array[6];
    block.x_center =    block_array[8];
    block.y_center =    block_array[10];
    block.width =       block_array[12];
    block.height =      block_array[14];
    block.angle =       block_array[16];
    block.tracking_index = block_array[18];
    block.age =          block_array[19];
    return block;
}

bool checksum_check_ccc(uint8_t byte_arr[])
{
    uint16_t sum, checksum;
    size_t i;
    checksum = byte_arr[4];
    sum = 0;
    for (i=0; i<20; i++)
        sum += byte_arr[i];
    return checksum == sum; 
}

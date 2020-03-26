#include "spi_pixy.h"

#define CCC_MSG 0xC1AE2002FF03

static SPI_Handle spi = NULL;
static Timer_Handle timer_pixy = NULL;
uint8_t request_packet_ccc [6] = {0xc1, 0xae, 32, 2, 0xFF, 0x03};
uint8_t request_packet_version [] = {0xc1, 0xae, 14, 0};
uint8_t receive_buffer[60];

void spi_pixy_init()
{
    SPI_Params spi_params;
    SPI_Params_init(&spi_params);
    spi_params.transferMode = SPI_MODE_BLOCKING;
    // spi_params.transferCallbackFxn = spi_pixy_callback;
    spi_params.frameFormat = SPI_POL1_PHA1;
    spi_params.bitRate = 2000000;
    spi_params.dataSize = 8;
    /*****************************/
    dbgOutputLoc(SPI_SPI_OPEN);
    /*****************************/
    spi = SPI_open(CONFIG_SPI_0, &spi_params);
    if (spi == NULL)
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

void * spiThread(void *arg0)
{
    
    memset((void *) receive_buffer, 0, 60);
    request_packet_ccc[0] = 0xC1;
    request_packet_ccc[1] = 0xAE;
    request_packet_ccc[2] = 0x20;
    request_packet_ccc[3] = 0x02;
    request_packet_ccc[4] = 0xFF;
    request_packet_ccc[5] = 0x03;
    char temp;
    uart_message_t uart_msg;
    uint8_t i, msg_size;
    while(1)
    {
        xQueueReceive(spi_start_q, &temp, portMAX_DELAY);

        SPI_Transaction spi_transaction;
        bool transferOK;
        spi_transaction.count = 4;
        spi_transaction.txBuf = (void *) request_packet_ccc;
        spi_transaction.rxBuf = (void *)receive_buffer;
        transferOK = SPI_transfer(spi, &spi_transaction);
        if (!transferOK)
            stop_all(FAIL_SPI_TRANSACTION);
          
        uint8_t num_blocks = spi_transaction.count / 20;
        if (num_blocks == 0) 
        {
            uart_msg.array_len = snprintf(uart_msg.msg, 100, "Not found, tc=%zu | ", spi_transaction.count);
            xQueueSendToBack(uart_debug_q, &uart_msg, 0);
        }
        Block_t block;

        for (i=0; i<1; i++)// num_blocks; i++)
        {
            block = convert_to_block_t(receive_buffer + (i * 20));

            msg_size = snprintf(uart_msg.msg, 100, "pixy,sig:%u,x:%u,y:%u,w:%u,h:%u",
                     block.signature, block.x_center, block.y_center,
                     block.width, block.height);
            uart_msg.array_len = msg_size;
            xQueueSendToBack(uart_debug_q, &uart_msg, 0);
        }
        
    }
}


void spi_pixy_callback(SPI_Handle handle, SPI_Transaction *transaction)
{
    
    /*****************************/
    dbgOutputLoc(SPI_PIXY_CALLBACK);
    /*****************************/
    uint8_t num_blocks = transaction->count / 20;
    Block_t block;
    uart_message_t uart_msg;
    size_t i = 0, msg_size;
    /*
    if (num_blocks == 0)
    {
        uart_msg.array_len = snprintf(uart_msg.msg, 100, "Not found, tc=%zu", transaction->count);
        xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    }
    */

    for (; i<num_blocks; i++)
    {
        block = convert_to_block_t(receive_buffer + (num_blocks * 20));

        msg_size = snprintf(uart_msg.msg, 100, "pixy,sig:%u,x:%u,y:%u,w:%u,h:%u",
                 block.signature, block.x_center, block.y_center,
                 block.width, block.height);
        uart_msg.array_len = msg_size;
        xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    }
}

void timer_spi_callback(Timer_Handle timer_handle)
{
    /*****************************/
    dbgOutputLoc(SPI_TIMER_CALLBACK);
    /*****************************/
    
    GPIO_toggle(CONFIG_GPIO_LED_0);
    /*
    uart_message_t uart_msg;
    uart_msg.array_len = 14;
    strncpy(uart_msg.msg, "timer callbacc", 14);
    xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    */
    char placeholder;
    xQueueSendFromISR(spi_start_q, &placeholder, NULL);
}


void send_pixy_ccc_spi()
{
    /*****************************/
    dbgOutputLoc(SPI_SEND_TRANSACTION);
    /*****************************/
    SPI_Transaction spi_transaction;
    bool transferOK;
    spi_transaction.count = 4 * 8;
    // TODO switch back
    // spi_transaction.txBuf = (void *)request_packet_version;
    spi_transaction.rxBuf = (void *)receive_buffer;
    transferOK = SPI_transfer(spi, &spi_transaction);
    if (!transferOK)
        stop_all(FAIL_SPI_TRANSACTION);
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

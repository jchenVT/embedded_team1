#include "spi_pixy.h"

void spi_pixy_init()
{
    SPI_init();
    SPI_Params_init(&spi_params);
    
    spi_params.transferMode = SPI_MODE_CALLBACK;
    spi_params.transferCallbackFxn = spi_pixy_callback;
    spi_params.frameFormat = SPI_POL1_PHA1;
    spi_params.bitRate = 2000000;
    spi_params.dataSize = 8;

    Timer_Params_init(&timer_pixy_params);
    timer_pixy_params.periodUnits = Timer_PERIOD_HZ;
    timer_pixy_params.period = 10;
    timer_pixy_params.timerCallback = timer_spi_callback;
    timer_pixy_params.timerMode = Timer_CONTINUOUS_CALLBACK;
    
    // TODO: set up timer 0 in syscfg
    timer_pixy = Timer_open(CONFIG_TIMER_0, &timer_pixy_params);
}


void spi_pixy_callback( SPI_Handle handle, SPI_Transaction *transaction )
{
    uint8_t num_blocks = transaction->count / 20;
    Block_t block;
    uart_message_t uart_msg;
    size_t i = 0, msg_size;
    if (num_blocks == 0)
    {
        uart_msg.array_len = 9;
        strncpy(uart_msg.msg, "Not found", 100);
        xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    }

    for (; i<num_blocks; i++)
    {
        block = convert_to_block_t(recv_packet_ccc + (num_blocks * 20));

        msg_size = snprintf(uart_msg.msg, 100, "pixy,sig:%u,x:%u,y:%u,w:%u,h:%u",
                 block.signature, block.x_center, block.y_center,
                 block.width, block.height);
        uart_msg.array_len = msg_size;
        xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    }
}

void timer_spi_callback(Timer_Handle timer_handle)
{
    uart_message_t uart_msg;
    uart_msg.array_len = 14;
    strncpy(uart_msg.msg, "timer callbacc", 100);
    xQueueSendFromISR(uart_debug_q, &uart_msg, NULL);
    send_pixy_ccc_spi();
}


void send_pixy_ccc_spi()
{
    SPI_Transaction spi_transaction;
    bool transferOK;
    spi_transaction.count = TX_MSGSIZE;
    spi_transaction.txBuf = (void *)request_packet_ccc;
    spi_transaction.rxBuf = (void *)recv_packet_ccc;
    transferOK = SPI_transfer(spi, &spi_transaction);
    if (!transferOK)
        // TODO: add some actual failure code
        stop_all(0xFA);
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

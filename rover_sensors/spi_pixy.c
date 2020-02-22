#include "spi_pixy.h"

void spi_pixy_init()
{
    SPI_init();
    SPI_Params_init(&spi_params);
    
    spi_params.transferMode = SPI_MODE_CALLBACK;
    spi_params.transferCallbackFxn = spi_pixy_callback;
    spi_params.bitRate = 2000000;
    spi_params.dataSize = 8;
}


void send_pixy_ccc_spi()
{
    SPI_Transaction spi_transaction;
    bool transferOK;
    spi_transaction.txBuf = (void *)request_packet_ccc;
    spi_transaction.rxBuf = (void *)recv_packet_ccc;
    transferOK = SPI_transfer(spi, &spi_transaction);
    if (!transferOK)
        // TODO: add some actual failure
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
    checksum = byte_arr[4];
    sum = 0;
    for (size_t i=0; i<20; i++)
        sum += byte_arr[i];
    return checksum == sum; 
}

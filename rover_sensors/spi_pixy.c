#include "spi_pixy.h"

#define CCC_MSG 0xC1AE2002FF03

#define RECEIVE_LENGTH 50

// static SPI_Handle spi = NULL;
static Timer_Handle timer_pixy = NULL;
uint8_t request_packet_ccc [10] = {0xc1, 0xae, 32, 2, 0xFF, 0x03};
uint8_t request_packet_version [] = {0xc1, 0xae, 14, 0};
uint8_t receive_buffer[RECEIVE_LENGTH];

void timerCallbackx(TimerHandle_t xTimer)
{
    return;
}

I2C_Handle spi_pixy_init()
{
    I2C_init();
    I2C_Handle i2c;
    I2C_Params i2c_params;
    I2C_Params_init(&i2c_params);
    i2c_params.transferMode = I2C_MODE_BLOCKING;
    i2c_params.bitRate = I2C_400kHz;

    i2c = I2C_open(CONFIG_I2C_0, &i2c_params);

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
            
    return i2c;
}

void * spiThread(void *arg0)
{
    char str[75];

    I2C_Handle i2c;
    i2c = spi_pixy_init();


    memset(receive_buffer, 0, 100);
    
    request_packet_ccc[0] = 0xc1;
    request_packet_ccc[1] = 0xae;
    request_packet_ccc[2] =   32;
    request_packet_ccc[3] =    2;
    request_packet_ccc[4] =  255;
    request_packet_ccc[5] =    1;
    char temp;
    
    const TickType_t xDelay = 10 / portTICK_PERIOD_MS;

    while(1)
    {
        xQueueReceive(spi_start_q, &temp, portMAX_DELAY);

        dbgOutputLoc(0x28);

        I2C_Transaction transaction_send = {0};
        transaction_send.slaveAddress = 0x10;
        transaction_send.writeBuf = request_packet_ccc;
        transaction_send.writeCount = 6;
        transaction_send.readCount = 0;
        
        bool success = false;
        success =  I2C_transfer(i2c, &transaction_send);
        
        snprintf(str, 75, "\ni2c write");
        UART_PRINT(str);
        
        if (!success) 
        {
            snprintf(str, 75, "\ni2c write failed");
            UART_PRINT(str);
            continue;
        }

        vTaskDelay ( xDelay);


        I2C_Transaction transaction_read = {0};
        transaction_read.writeCount = 0;
        transaction_read.readBuf = receive_buffer;
        transaction_read.readCount = RECEIVE_LENGTH;
        success =  I2C_transfer(i2c, &transaction_read);

        snprintf(str, 75, "\ni2c read");
        UART_PRINT(str);

        
        if (!success) 
        {
            snprintf(str, 75, " ,i2c read failed");
            UART_PRINT(str);
            continue;
        }
        
        /*
        transaction.txBuf = (void *) request_packet_ccc;
        transaction.rxBuf = (void *)receive_buffer;
        dbgOutputLoc(0x29);
        transferOK = SPI_transfer(spi, &spi_transaction);
        dbgOutputLoc(0x2A);
        if (!transferOK)
        {
            dbgOutputLoc(0x7A);
            stop_all(FAIL_SPI_TRANSACTION);
        }
        */
          
        Block_t block;

        int block_start = 0;
        uint8_t check;
        while (block_start < RECEIVE_LENGTH)
        {
            check = receive_buffer[block_start];
            if ( check == 0xc1 )
            {
                check = receive_buffer[block_start+1];
                if ( check == 0xaf )
                {
                    snprintf(str, 75, "\nfound c1af at %d", block_start );
                    UART_PRINT(str);
                    break;
                } 
            }
            else
                block_start++;
        }
        if (block_start == RECEIVE_LENGTH)
        {
            snprintf(str, 75, "\nblock not found");
            UART_PRINT(str);
            continue;
        }

        block = convert_to_block_t(&receive_buffer[block_start]);
        bool checksum_good = checksum_check_ccc(receive_buffer + block_start);
        
        snprintf(str, 75, "\npixy,sig:%u,x:%u,y:%u,w:%u,h:%u, checksum:%s",
                    block.signature, block.x_center, block.y_center,
                    block.width, block.height, checksum_good ? "true" : "false");
        UART_PRINT(str);
        
    }
}



void timer_spi_callback(Timer_Handle timer_handle)
{
    /*****************************/
    dbgOutputLoc(SPI_TIMER_CALLBACK);
    /*****************************/
    
    char placeholder = 0;
    xQueueSendFromISR(spi_start_q, &placeholder, 0);
}


void send_pixy_ccc_spi()
{
    /*****************************
    dbgOutputLoc(SPI_SEND_TRANSACTION);
    //SPI_Transaction spi_transaction;
    bool transferOK;
    spi_transaction.count = 4 * 8;
    // TODO switch back
    // spi_transaction.txBuf = (void *)request_packet_version;
    //spi_transaction.rxBuf = (void *)receive_buffer;
    //transferOK = SPI_transfer(spi, &spi_transaction);
    if (!transferOK)
        stop_all(FAIL_SPI_TRANSACTION);

   //  I2C_transfer(i2c, &i2c_transaction);
    *****************************/
}

Block_t convert_to_block_t(uint8_t block_array[])
{
    Block_t block;
    block.signature =   (block_array[6] << 8) | block_array[7];
    block.x_center =    (block_array[9] << 8) | block_array[8];
    block.y_center =    (block_array[11] << 8) | block_array[10];
    block.width =       (block_array[13] << 8) | block_array[12];
    block.height =      (block_array[15] << 8) | block_array[14];
    /*
    block.angle =       block_array[16];
    block.tracking_index = block_array[18];
    block.age =          block_array[19];
    */
    return block;
}

bool checksum_check_ccc(uint8_t byte_arr[])
{
    /*
    uint16_t sum, checksum;
    size_t i;
    checksum = (byte_arr[4] << 4) + byte_arr[5];
    sum = 0;
    for (i=6; i<20; i++)
        sum += byte_arr[i];
    return checksum == sum; 
    */
    char str[30];
    uint8_t packet_type = byte_arr[2];
    snprintf(str, 30, "\npacket type:%d", packet_type);
    UART_PRINT(str);
    return byte_arr[2] ==  0x33;
}

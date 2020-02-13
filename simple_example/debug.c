#include "debug.h"

UART_Handle uart = NULL;
UART_Params uartParams;
static QueueHandle_t uartQ = NULL;

void debug_setup()
{
    GPIO_init();
    GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_4, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_5, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_6, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_7, GPIO_CFG_OUTPUT);

    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, GPIO_CFG_OUT_LOW);

    /*****************************/
    dbgOutputLoc(UART_INITIALIZE);
    /*****************************/

    UART_init();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    /*****************************/
    dbgOutputLoc(UART_OPENING);
    /*****************************/

    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

    // creating uart queue
    uartQ = xQueueCreate( 32, 1);
    if (uartQ == NULL) stop_all(FAIL_UART_INIT);
}

void dbgOutputLoc(unsigned int outLoc)
{
    if (outLoc > 0x7B)
        stop_all(FAIL_UNKNOWN_CODE);
    
    static bool firstbit = false;
    firstbit = !firstbit;
    GPIO_write(CONFIG_GPIO_0, firstbit ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, outLoc & 0x01 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, outLoc & 0x02 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, outLoc & 0x04 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, outLoc & 0x08 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, outLoc & 0x10 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, outLoc & 0x20 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, outLoc & 0x40 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
}

void dbgUARTVal(unsigned char outVal)
{
    /*****************************
    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

    const char e[0] = outVal;
    //snprintf(e, 5, "%u\n", outVal);

    dbgOutputLoc(UART_WRITING); 

    UART_write(uart, e, sizeof(e));

    *****************************/
    xQueueSendToBackFromISR( uartQ, &outVal, 0);
}

void stop_all(unsigned int FAILURE_CODE)
{
    //taskDISABLE_INTERRUPTS();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /*****************************/
    dbgOutputLoc(UART_CLOSING);
    /*****************************/
    UART_close(uart);
    /*****************************/
    dbgOutputLoc(FAILURE_CODE);
    /*****************************/

    while(1)
    {
        GPIO_toggle(CONFIG_GPIO_LED_0);
        // blink LED forever
        int i = 0;
        for (;i<100000;i++) ;
    }
}


unsigned char receiveFromUARTQ() 
{

    unsigned char msg = 0;

    xQueueReceive( uartQ, &msg, portMAX_DELAY );

    if (&msg == NULL) 
        return (unsigned char) -1;
    
    return msg;
}

void *uartThread(void *arg0)
{
    while(1)
    {
        unsigned char outVal = receiveFromUARTQ(); 

        if ((int) outVal == -1)
            stop_all(FAIL_UART_INIT);
        
        if (uart == NULL)
            stop_all(FAIL_UART_INIT);

        const char e[1] = {outVal};

        dbgOutputLoc(UART_WRITING); 

        UART_write(uart, e, sizeof(e));
    }
}

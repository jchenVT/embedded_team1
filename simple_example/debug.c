#include "debug.h"

UART_Handle uart;
UART_Params uartParams;

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
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    /*****************************/
    dbgOutputLoc(UART_OPENING);
    /*****************************/

    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL)
        stop_all();
}

void dbgOutputLoc(unsigned int outLoc)
{
    if (outLoc > 0x7F)
        stop_all();
    
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
    if (uart == NULL)
        stop_all();

    /*****************************/
    dbgOutputLoc(UART_WRITING); 
    /*****************************/
    UART_write(uart, (const void *) outVal, sizeof(outVal));
}

void stop_all()
{
    taskDISABLE_INTERRUPTS();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /*****************************/
    dbgOutputLoc(UART_CLOSING);
    dbgOutputLoc(FAILURE);
    /*****************************/
    UART_close(uart);

    while(1)
    {
        GPIO_toggle(CONFIG_GPIO_LED_0);
        // blink LED forever
        int i = 0;
        for (;i<100000;i++) ;
    }
}

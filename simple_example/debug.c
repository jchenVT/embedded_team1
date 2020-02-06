#include "debug.h"

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

    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_1, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_2, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_3, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_4, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_5, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_6, GPIO_CFG_OUT_HIGH);
    GPIO_write(CONFIG_GPIO_7, GPIO_CFG_OUT_HIGH);
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
    UART_Handle uart;
    UART_Params uartParams;
    
    /*
    static UART_Handle uart;
    static UART_Params uartParams;
    static bool initialized = false;
    
    if (!initialized)
    {
        UART_init();
        GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        UART_Params_init(&uartParams);
        uartParams.writeDataMode = UART_DATA_BINARY;
        uartParams.readDataMode = UART_DATA_BINARY;
        uartParams.readReturnMode = UART_RETURN_FULL;
        uartParams.readEcho = UART_ECHO_OFF;
        uartParams.baudRate = 115200;

        uart = UART_open(CONFIG_UART_0, &uartParams);
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);

        if (uart == NULL) { // uart failed to initialize
            stop_all();
        }

        initialized = true;
    }
    */

    UART_init();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL)
        stop_all();
    UART_write(uart, (const void *) outVal, sizeof(outVal));
    UART_close(uart);

}

void stop_all()
{
    //taskDISABLE_INTERRUPTS();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    while(1)
    {
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        // blink LED forever
        int i = 0;
        for (;i<100000;i++) ;
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    }
}

#ifndef _TEAM1_DEBUG_
#define _TEAM1_DEBUG_

#include <FreeRTOS.h>
#include <task.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdbool.h>

#include "ti_drivers_config.h"

// Debug Output Location Macros Definition
// Used in dbgOUtputLoc(_MACRO_)
#define FAILURE 0x7F


// Set up function
void debug_setup();

// Send a value through UART
void dbgUARTVal(unsigned char outVal);

// Debug function that outputs to GPIO pins
// Use logic analyzer to determine location
// outLoc must be <= 127
void dbgOutputLoc(unsigned int outLoc);

void stop_all(void);

#endif

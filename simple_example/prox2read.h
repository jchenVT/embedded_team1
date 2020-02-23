/*
 *  prox2read.h
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#ifndef PROX2READ_H_
#define PROX2READ_H_

#include <ti/drivers/GPIO.h>

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include "ti_drivers_config.h"

/* Queue files */
// here

/* Debug files */
#include "debug.h"


/* Main Thread */
void readProximitySensor();

#endif /* PROX2READ_H_ */

#ifndef STAR_H_
#define STAR_H_

/* Timer */
#include <ti/drivers/Timer.h>
#include <FreeRTOS.h>
#include <timers.h>

/* JSON formatting used to push to msg queues */
#include "jsonFormat.h"

#define SUB_TOPIC   0
#define PUB_TOPIC   1

void *starThread(void *arg0);
void timerCallback(TimerHandle_t xTimer);

#endif /* STAR_H_ */

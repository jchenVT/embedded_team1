#ifndef STAR_H_
#define STAR_H_

/* Timer */
#include <ti/drivers/timer.h>
#include <timers.h>

/* Message queues */
#include <mqtt_queue.h>

#define SUB_TOPIC   0
#define PUB_TOPIC   1

void *starThread(void *arg0);
void timerCallback(TimerHandle_t xTimer);

#endif /* STAR_H_ */

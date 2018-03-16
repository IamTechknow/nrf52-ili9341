#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#include "nrf52832_dev.h"

#define MIN_THRESHOLD 10
#define MAX_THRESHOLD 1000
#define RESISTANCE 300

//API functions
uint16_t getADCValue(void);

void ssadc_init(void);

uint16_t readTouchX(void);

uint16_t readTouchY(void);

uint16_t pressure(void);

#endif
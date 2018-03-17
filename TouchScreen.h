#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#include "nrf52832_dev.h"

//Min pressure needed to detect touch
#define MIN_THRESHOLD 800

//Offsets to account for ADC values and to map result from ADC range to desired range
#define X_OFFSET 415
#define Y_OFFSET 445
#define X_MAX 320
#define Y_MAX 240
#define RAW_X_MAX 350
#define RAW_Y_MAX 290

//API functions
uint16_t getADCValue(nrf_saadc_input_t in);

void saadc_init(void);

uint16_t readTouchX(void);

uint16_t readTouchY(void);

uint16_t pressure(void);

int16_t mapX(int raw);

int16_t mapY(int raw);

#endif
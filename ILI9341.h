#ifndef _ILI9341_H_
#define _ILI9341_H_

#include "nrf52832_dev.h"

//Dimensions
#define TFTWIDTH   240
#define TFTHEIGHT  320

//Internal functions
void setReadDirInline(void);

void setWriteDirInline(void);

void write8(uint8_t data);

void wr_strobe(void);

uint8_t read8(void);

uint32_t readReg(uint8_t addr);

void flood(uint16_t color, uint32_t len);

//API functions
uint16_t getId(void);

void fillScreen(uint16_t color);

void gpio_init(void);

#endif

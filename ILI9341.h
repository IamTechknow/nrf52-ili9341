#ifndef _ILI9341_H_
#define _ILI9341_H_

#include "nrf52832_dev.h"

//Dimensions
#define TFTWIDTH   240
#define TFTHEIGHT  320

//Registers
#define ILI9341_NOOP               0x00
#define ILI9341_SOFTRESET          0x01
#define ILI9341_SLEEPIN            0x10
#define ILI9341_SLEEPOUT           0x11
#define ILI9341_NORMALDISP         0x13
#define ILI9341_INVERTOFF          0x20
#define ILI9341_INVERTON           0x21
#define ILI9341_GAMMASET           0x26
#define ILI9341_DISPLAYOFF         0x28
#define ILI9341_DISPLAYON          0x29
#define ILI9341_COLADDRSET         0x2A
#define ILI9341_PAGEADDRSET        0x2B
#define ILI9341_MEMORYWRITE        0x2C
#define ILI9341_PIXELFORMAT        0x3A
#define ILI9341_FRAMECONTROL       0xB1
#define ILI9341_DISPLAYFUNC        0xB6
#define ILI9341_ENTRYMODE          0xB7
#define ILI9341_POWERCONTROL1      0xC0
#define ILI9341_POWERCONTROL2      0xC1
#define ILI9341_VCOMCONTROL1      0xC5
#define ILI9341_VCOMCONTROL2      0xC7
#define ILI9341_MEMCONTROL      0x36
#define ILI9341_MADCTL  0x36
#define ILI9341_ID4  0xD3

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

//Internal functions
void setReadDirInline(void);

void setWriteDirInline(void);

void wr_strobe(void);

void write8(uint8_t data);

void writeRegister8inline(uint8_t addr, uint8_t data);

void writeRegister16inline(uint8_t addr, uint16_t data);

void writeRegister32(uint8_t addr, uint32_t data);

uint8_t read8(void);

uint32_t readReg(uint8_t addr);

void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void flood(uint16_t color, uint32_t len);

//API functions
void lcd_reset(void);

void lcd_init(void);

uint16_t getId(void);

void fillScreen(uint16_t color);

void gpio_init(void);

#endif

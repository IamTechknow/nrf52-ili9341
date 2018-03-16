/*********************************************************************
This is a port of the ILI9341 driver to the Nordic nRF52 series.
The Nordic driver only supports SPI, while this one supports a 8-bit data bus on GPIO.
Here are the pin declarations needed which should be compatiable for
the nRF52832 and nRF52840 development kits.
*********************************************************************/

#ifndef _NRF52832_DEV_H_
#define _NRF52832_DEV_H_

#include "boards.h" //contains pin declarations
#include "nrf_gpio.h"

//Pin declarations. Only 8 data pins are exposed, so the Interface Mode pins should already
//be set to the 80-system 8-bit interface (mode 3)
#define LCD_RESET ARDUINO_A4_PIN // Can alternately just connect to Arduino's reset pin
#define LCD_CS ARDUINO_A3_PIN // Chip Select goes to Analog 3
#define LCD_CD ARDUINO_A2_PIN // Command/Data (CD) goes to Analog 2. C = low
#define LCD_WR ARDUINO_A1_PIN // LCD Write goes to Analog 1
#define LCD_RD ARDUINO_A0_PIN // LCD Read goes to Analog 0

//Data pins that may be used for both reading and writing data.
//The pin direction must be set before and after reading data, because write operations happen more often.
#define LCD_D0 ARDUINO_8_PIN
#define LCD_D1 ARDUINO_9_PIN
#define LCD_D2 ARDUINO_2_PIN
#define LCD_D3 ARDUINO_3_PIN
#define LCD_D4 ARDUINO_4_PIN
#define LCD_D5 ARDUINO_5_PIN
#define LCD_D6 ARDUINO_6_PIN
#define LCD_D7 ARDUINO_7_PIN

//Touchscreen pins, which correspond to some of the screen pins
#define X_PLUS ARDUINO_9_PIN
#define X_MINUS ARDUINO_A3_PIN
#define Y_PLUS ARDUINO_A2_PIN
#define Y_MINUS ARDUINO_8_PIN

#endif

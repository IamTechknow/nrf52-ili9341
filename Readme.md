# ILI9341 driver port for nRF52 series devices

This is a C driver port of the Adafruit TFT Touch Screen shield coded for the Arduino Uno and Mega, for the nRF52 series SoCs (primarily the nRF52832 development kit but the nRF52840 development kit should also work.) Specifically these drivers are designed to work with the [Foxnovo TFT Touch Screen Arduino shield](https://www.amazon.com/Touch-Screen-Socket-Arduino-Module/dp/B00UAA2XIC) which may be purchased for cheap from Amazon.

## Display Information
The touch screen has pin compatiability with the nRF52 development kits, as the pin layout is the same as for an Arduino Uno. It uses a ILITEK ILI9341 LCD chip  controller. Screens that would sold earlier than November 2015 (when I purchased mine) may have the LGDP4535 driver instead, this library is NOT compatiable with that but may be modified to be so. The MCU interface is set to a parallel 8-bit data bus, and therefore requires 8 pins for data, and 5 more pins for control. Also note that it is not possible to change or turn off the backlight by accessing the correct register, because the backlight control pin is permanently connected to VCC.

The touch screen of course contains a resistive touch sensor and a SD card slot accessible by a SPI interface. The drivers include support for the touch sensor, but have yet to be integrated with the display driver, so they can't be used together in a client program yet. No data sheet is available for the touch sensor, which is unsuprising for such a product.

## Attributions
These drivers use a [nRF52 port of the Adafruit GFX library](https://github.com/electronut/ElectronutLabs-bluey/tree/master/code/bluey-OLED) to the open-source bluey platform, made by Electronut Labs. The code to port from may be found at Adafruit's [repo](https://github.com/adafruit/TFTLCD-Library/).

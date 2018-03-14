#include <stdlib.h>
#include <string.h>
#include "nrf_delay.h"
#include "ILI9341.h"

//Global variables
uint8_t gpio_out[5] = {LCD_RESET, LCD_CS, LCD_CD, LCD_WR, LCD_RD};
uint8_t gpio_data[8] = {LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7};

/**@brief Set the pin direction for the data pins as inputs for a read operation.
 */
void setReadDirInline(void) {
	for(uint8_t i = 0; i < 8; i++) {
		nrf_gpio_pin_dir_set(gpio_data[i], NRF_GPIO_PIN_DIR_INPUT);
		nrf_gpio_cfg_input(gpio_data[i], NRF_GPIO_PIN_PULLUP);
	}
}

/**@brief Set the pin direction for the data pins as outputs for a read operation.
 */
void setWriteDirInline(void) {
	for(uint8_t i = 0; i < 8; i++) 
		nrf_gpio_pin_dir_set(gpio_data[i], NRF_GPIO_PIN_DIR_OUTPUT);
}

/**@brief Data write strobe, ~2 instructions and always inline. See page 31 in spec
 */
void wr_strobe() {
	nrf_gpio_pin_write(LCD_WR, 0);
	nrf_gpio_pin_write(LCD_WR, 1);
}

/**@brief Write data to the 8-bit data bus. TODO: This should be done as a bit mask instead.
 */
void write8(uint8_t data) {
	nrf_gpio_pin_write(LCD_D0, data & 0x01);
	nrf_gpio_pin_write(LCD_D1, (data & 0x02) >> 1);
	nrf_gpio_pin_write(LCD_D2, (data & 0x04) >> 2);
	nrf_gpio_pin_write(LCD_D3, (data & 0x08) >> 3);
	nrf_gpio_pin_write(LCD_D4, (data & 0x10) >> 4);
	nrf_gpio_pin_write(LCD_D5, (data & 0x20) >> 5);
	nrf_gpio_pin_write(LCD_D6, (data & 0x40) >> 6);
	nrf_gpio_pin_write(LCD_D7, (data & 0x80) >> 7);
	wr_strobe();
}

/**@brief Read data from the 8-bit data bus. This requires a minimum 400ns delay, see page 32 in spec
 */
uint8_t read8() {
	uint8_t val = 0;
	
	//Enable read op and wait
	nrf_gpio_pin_write(LCD_RD, 0);
	nrf_delay_us(1);
	
	val |= nrf_gpio_pin_read(LCD_D0);
	val |= nrf_gpio_pin_read(LCD_D1) << 1;
	val |= nrf_gpio_pin_read(LCD_D2) << 2;
	val |= nrf_gpio_pin_read(LCD_D3) << 3;
	val |= nrf_gpio_pin_read(LCD_D4) << 4;
	val |= nrf_gpio_pin_read(LCD_D5) << 5;
	val |= nrf_gpio_pin_read(LCD_D6) << 6;
	val |= nrf_gpio_pin_read(LCD_D7) << 7;
	
	nrf_gpio_pin_write(LCD_RD, 1);
	return val;
}

/**@brief Since this is a read operation,
		  the pin direction is changed, data is read, then direction is changed back.
 */
uint32_t readReg(uint8_t addr) {
	uint32_t val = 0;
	
	nrf_gpio_pin_write(LCD_CS, 0);
	nrf_gpio_pin_write(LCD_CD, 0);
	write8(addr);
	setReadDirInline();
	nrf_gpio_pin_write(LCD_CD, 1); //select status reg
	nrf_delay_us(50);
	
	for(int i = 0; i < 3; i++) { //Get the most sig 24-bits and shift them to right place
		val |= read8();
		val <<= 8; 
	}
	val |= read8(); //get the last 8 bits
	nrf_gpio_pin_write(LCD_CS, 1);
	setWriteDirInline();
	return val;	
}

void flood(uint16_t color, uint32_t len) {
	
}

/**@brief Function for initializing the GPIO pins as inputs and outputs
 */
void gpio_init(void) {
	//The control signals are active low so set them to high.
	for(uint8_t i = 0; i < 5; i++) {
		nrf_gpio_pin_dir_set(gpio_out[i], NRF_GPIO_PIN_DIR_OUTPUT);
		nrf_gpio_pin_write(gpio_out[i], 1);
	}
	
	setWriteDirInline();
}

/**@brief Read the device code of the LCD. 
		  For the ILI9341, the device code is in the ID4 register.
 */
uint16_t getId(void) {
	return (uint16_t) (readReg(0xD3) & 0xffff); //use lower 16-bit mask
}

/**@brief Fill the screen with the specified color.
 */
void fillScreen(uint16_t color) {
	
}

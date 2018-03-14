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

/**@brief Write command to the 8-bit data bus. TODO: This should be done as a bit mask instead.
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

/**@brief Write command to the 8-bit data bus, and the argument
 */
void writeRegister8inline(uint8_t addr, uint8_t data) {
	nrf_gpio_pin_write(LCD_CD, 0);
	write8(addr);
	nrf_gpio_pin_write(LCD_CD, 1);
	write8(data);
}

/**@brief Write command (padded with zeros) to the 8-bit data bus, and a 16-bit argument
 */
void writeRegister16inline(uint8_t addr, uint16_t data) {
	nrf_gpio_pin_write(LCD_CD, 0);
	write8(0);
	write8(addr);
	nrf_gpio_pin_write(LCD_CD, 1);
	write8(data >> 8);
	write8(data);
}

void writeRegister32(uint8_t addr, uint32_t data) {
	nrf_gpio_pin_write(LCD_CD, 0);
	write8(addr);
	nrf_gpio_pin_write(LCD_CD, 1);
	nrf_delay_us(10);
	write8(data >> 24);
	nrf_delay_us(10);
	write8(data >> 16);
	nrf_delay_us(10);
	write8(data >> 8);
	nrf_delay_us(10);
	write8(data);
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

/**@brief Sets the LCD address window.
   Input coordinates are assumed pre-sorted (e.g. x2 >= x1)
*/
void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	uint32_t t;
	nrf_gpio_pin_write(LCD_CS, 0);

	t = x1 << 16;
    t |= x2;
    writeRegister32(ILI9341_COLADDRSET, t);
    t = y1 << 16;
    t |= y2;
    writeRegister32(ILI9341_PAGEADDRSET, t);

	nrf_gpio_pin_write(LCD_CS, 1);
}

void flood(uint16_t color, uint32_t len) {

}

void lcd_reset(void) {
	for(int i = 1; i < 5; i++)
		nrf_gpio_pin_write(gpio_out[i], 1);

	//Trigger reset
	nrf_gpio_pin_write(LCD_RESET, 0);
	nrf_delay_ms(2);
	nrf_gpio_pin_write(LCD_RESET, 1);

	//Data transfer sync
	nrf_gpio_pin_write(LCD_CS, 0);
	nrf_gpio_pin_write(LCD_CD, 0);
	write8(ILI9341_NOOP);
	for(int i = 0; i < 3; i++)
		wr_strobe();
	nrf_gpio_pin_write(LCD_CS, 1);

	nrf_delay_ms(250);
}

/**@brief Initialize the LCD. Set control registers and
   enable the entire screen to be addressable.
*/
void lcd_init(void) {
	nrf_gpio_pin_write(LCD_CS, 0);

	writeRegister8inline(ILI9341_SOFTRESET, 0);
	nrf_delay_ms(50);
	writeRegister8inline(ILI9341_DISPLAYOFF, 0);

	writeRegister8inline(ILI9341_POWERCONTROL1, 0x23);
	writeRegister8inline(ILI9341_POWERCONTROL2, 0x10);
	writeRegister16inline(ILI9341_VCOMCONTROL1, 0x2B2B);
	writeRegister8inline(ILI9341_VCOMCONTROL1, 0xC0);
	writeRegister8inline(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
	writeRegister8inline(ILI9341_PIXELFORMAT, 0x55);
	writeRegister16inline(ILI9341_FRAMECONTROL, 0x001B);

	writeRegister8inline(ILI9341_ENTRYMODE, 0x07);

	writeRegister8inline(ILI9341_SLEEPOUT, 0);
	nrf_delay_ms(150);
	writeRegister8inline(ILI9341_DISPLAYON, 0);
	nrf_delay_ms(250);

	setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
	nrf_gpio_pin_write(LCD_CS, 1);
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
	return (uint16_t) (readReg(ILI9341_ID4) & 0xffff); //use lower 16-bit mask
}

/**@brief Fill the screen with the specified color.
 */
void fillScreen(uint16_t color) {

}

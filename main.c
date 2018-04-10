/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ILI9341.h"
#include "Adafruit_GFX.h"
#include "TouchScreen.h"

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define GRAPHICS_TEST

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE 0xFFFF

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

#ifdef GRAPHICS_TEST
void testFillScreen() {
	ILI9341_fillScreen(BLACK);
	ILI9341_fillScreen(RED);
	ILI9341_fillScreen(GREEN);
	ILI9341_fillScreen(BLUE);
	ILI9341_fillScreen(BLACK);
}

void testLines(uint16_t color) {
	uint16_t x1, y1, x2, y2, w = TFTWIDTH, h = TFTHEIGHT;

	ILI9341_fillScreen(BLACK);
	x1 = y1 = 0;
	y2 = h - 1;

	for(x2 = 0; x2 < w; x2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for(y2 = 0; y2 < h; y2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);

	ILI9341_fillScreen(BLACK);
	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;
	for(x2 = 0; x2 < w; x2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for(y2 = 0; y2 < h; y2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);

	ILI9341_fillScreen(BLACK);
	x1 = y2 = 0;
	y1 = h - 1;
	for(x2 = 0; x2 < w; x2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for(y2 = 0; y2 < h; y2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);

	ILI9341_fillScreen(BLACK);
	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;
	for(x2 = 0; x2 < w; x2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);
	
	for(x2 = 0; y2 < h; y2 += 6) 
		Adafruit_GFX_drawLine(x1, y1, x2, y2, color);
}

void testFastLines(uint16_t color1, uint16_t color2) {
	uint16_t w = TFTWIDTH, h = TFTHEIGHT;
	ILI9341_fillScreen(BLACK);
	
	for(uint16_t y = 0; y < h; y += 5) 
		Adafruit_GFX_drawFastHLine(0, y, w, color1);
	for(uint16_t x = 0; x < w; x += 5) 
		Adafruit_GFX_drawFastVLine(x, 0, h, color2);
}

void testRects(uint16_t color) {
	uint16_t cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	uint16_t n = TFTWIDTH <= TFTHEIGHT ? TFTWIDTH : TFTHEIGHT;

	for(uint16_t i = 2; i < n; i += 6) {
		uint8_t j = i >> 1;
		Adafruit_GFX_drawRect(cx - j, cy - j, i, i, color);
	}
}

void testFilledRects(uint16_t color1, uint16_t color2) {
	uint16_t cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	uint16_t n = TFTWIDTH <= TFTHEIGHT ? TFTWIDTH : TFTHEIGHT;

	ILI9341_fillScreen(BLACK);
	for(int i = n; i > 0; i -= 6) {
		uint8_t j = i >> 1;
		Adafruit_GFX_fillRect(cx - j, cy - j, i, i, color1);
		Adafruit_GFX_drawRect(cx - j, cy - j, i, i, color2);
	}
}

void testFilledCircles(uint8_t radius, uint16_t color) {
	int x, y, w = TFTWIDTH, h = TFTHEIGHT, r2 = radius << 1;
	ILI9341_fillScreen(BLACK);

	for(x = radius; x < w; x += r2) 
		for(y = radius; y < h; y += r2) 
			Adafruit_GFX_fillCircle(x, y, radius, color);
}

void testCircles(uint8_t radius, uint16_t color) {
	int x, y, w = TFTWIDTH + radius , h = TFTHEIGHT + radius , r2 = radius << 1;

	for(x = 0; x < w; x += r2) 
		for(y = 0; y < h; y += r2) 
			Adafruit_GFX_drawCircle(x, y, radius, color);
}

void testTriangles() {
	int cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	int n = cx <= cy ? cx : cy;
	ILI9341_fillScreen(BLACK);

	for(int i = 0; i < n; i += 5) 
		Adafruit_GFX_drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, 0x0FFF);
}

void testFilledTriangles() {
	int cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	ILI9341_fillScreen(BLACK);
	int limit = cx <= cy ? cx : cy;

	for(int i = 10; i < limit; i += 5) {
		Adafruit_GFX_fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, 0x0FFF);

		Adafruit_GFX_fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, 0x0FFF);
	}
}

void testRoundRects() {
	int cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	ILI9341_fillScreen(BLACK);
	int w = TFTWIDTH <= TFTHEIGHT ? TFTWIDTH : TFTHEIGHT, i, i2;

	for(i = 0; i < w; i += 6) {
		i2 = i >> 1;
		Adafruit_GFX_drawRoundRect(cx - i2, cy - i2, i, i, i >> 3, 0x0FFF);
	}
}

void testFilledRoundRects() {
	int cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1, i2;
	ILI9341_fillScreen(BLACK);
	int limit = TFTWIDTH <= TFTHEIGHT ? TFTWIDTH : TFTHEIGHT;

	for(int i = 20; i < limit; i += 6) {
		i2 = i >> 1;
		Adafruit_GFX_fillRoundRect(cx - i2, cy - i2, i, i, i >> 3, 0x0FFF);
	}
}

void testText() {
	char *hello = "Hello World!", *num = "1234.56", *hex = "DEADBEEF", *groop = "Groop";
	const char *txt[8] = {"I implore thee,", "my foonting turlingdromes.", "And hooptiously drangle me",
	"with crinkly bindlewurdles,", "Or I will rend thee", "in the gobberwarts",
	"with my blurglecruncheon,", "see if I don't!"};

	ILI9341_fillScreen(BLACK);
	Adafruit_GFX_setCursor(0, 0);
	
	//Print hello world with white color, black background, default size
	Adafruit_GFX_setTextColor(WHITE, BLACK);
	Adafruit_GFX_setTextSize(1);
	for(int i = 0; i < 13; i++)
		Adafruit_GFX_write(*(hello+i));
	Adafruit_GFX_write('\n');

	Adafruit_GFX_setTextColor(YELLOW, BLACK);
	Adafruit_GFX_setTextSize(2);
	for(int i = 0; i < 7; i++)
		Adafruit_GFX_write(*(num+i));
	Adafruit_GFX_write('\n');

	Adafruit_GFX_setTextColor(RED, BLACK);
	Adafruit_GFX_setTextSize(3);
	for(int i = 0; i < 10; i++)
		Adafruit_GFX_write(*(hex+i));
	Adafruit_GFX_write('\n');
	Adafruit_GFX_write('\n');
	
	Adafruit_GFX_setTextColor(GREEN, BLACK);
	Adafruit_GFX_setTextSize(5);
	for(int i = 0; i < 5; i++)
		Adafruit_GFX_write(*(groop+i));
	Adafruit_GFX_write('\n');
	
	Adafruit_GFX_setTextSize(2);
	for(int i = 0; i < 15; i++)
		Adafruit_GFX_write(*(txt[0]+i));
	Adafruit_GFX_write('\n');
	
	Adafruit_GFX_setTextSize(1);
	for(int i = 1; i < 8; i++) {
		int len = strlen(txt[i]);
		for(int j = 0; j < len; j++)
			Adafruit_GFX_write(*(txt[i]+j));
		Adafruit_GFX_write('\n');
	}
}

void rtc_init(void) {
	// Start LFCLK (32kHz) crystal oscillator. If you don't have crystal on your board, choose RCOSC instead.
	NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	
	// 1kHz RTC frequency
	NRF_RTC0->PRESCALER = 31;
	
	// Start RTC
	NRF_RTC0->TASKS_START = 1;
}

void rtc_stop(void) {
	NRF_RTC0->TASKS_STOP = 1;
	NRF_CLOCK->TASKS_LFCLKSTOP = 1;
}
#endif

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
	gpio_init();
	lcd_reset();
	lcd_init();
	Adafruit_GFX_init(TFTWIDTH, TFTHEIGHT, ILI9341_drawPixel);

    // Start execution.
    NRF_LOG_INFO("LCD Test started.");
	NRF_LOG_INFO("LCD ID: %x", getId());
	
#ifdef GRAPHICS_TEST
	rtc_init();
	NRF_LOG_INFO("Benchmark                Time (milliseconds)");
	uint32_t time;
	
	//For each test, measure the time each test takes in milliseconds.
	
	time = NRF_RTC0->COUNTER;
	testFillScreen();
	NRF_LOG_INFO("Screen fill              %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testText();
	NRF_LOG_INFO("Text                     %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testLines(CYAN);
	NRF_LOG_INFO("Lines                    %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testFastLines(RED, BLUE);
	NRF_LOG_INFO("Horiz/Vert Lines         %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testRects(GREEN);
	NRF_LOG_INFO("Rectangles (outline)     %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testFilledRects(YELLOW, MAGENTA);
	NRF_LOG_INFO("Rectangles (filled)      %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testFilledCircles(10, MAGENTA);
	NRF_LOG_INFO("Circles (filled)         %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testCircles(10, WHITE);
	NRF_LOG_INFO("Circles (outline)        %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testTriangles();
	NRF_LOG_INFO("Triangles (outline)      %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testFilledTriangles();
	NRF_LOG_INFO("Triangles (filled)       %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testRoundRects();
	NRF_LOG_INFO("Rounded rects (outline)  %d", NRF_RTC0->COUNTER - time);
	
	time = NRF_RTC0->COUNTER;
	testFilledRoundRects();
	NRF_LOG_INFO("Rounded rects (filled)   %d", NRF_RTC0->COUNTER - time);
	rtc_stop();
	
	uint8_t i = 0;
#else
	saadc_init();
#endif
	
    // Enter main loop.
    for (;;)
    {
#ifdef GRAPHICS_TEST
		Adafruit_GFX_setRotation(i % 4);
		ILI9341_setRotation(i++ % 4);
		testText();
		nrf_delay_ms(2000);
#else
		int16_t x = readTouchX(), y = readTouchY(), z = pressure();
		if(z > MIN_THRESHOLD && x >= 0 && y >= 0)
			NRF_LOG_INFO("X: %d, Y: %d, Pressure = %d", x, y, z);
		nrf_delay_ms(100);
#endif
    }
}


/**
 * @}
 */

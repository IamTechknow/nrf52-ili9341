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

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

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

void testTriangles() {
	int cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	int n = cx <= cy ? cx : cy;
	
	Adafruit_GFX_fillScreen(0);
	for(int i = 0; i < n; i += 5) 
		Adafruit_GFX_drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, 0x0FFF);
}

void testFilledRects(uint16_t color1, uint16_t color2) {
	int cx = (TFTWIDTH >> 1) - 1, cy = (TFTHEIGHT >> 1) - 1;
	int n = cx <= cy ? cx : cy;
	
	Adafruit_GFX_fillScreen(0);
	for(int i = n; i > 0; i -= 6) {
		uint8_t j = i >> 1;
		Adafruit_GFX_drawRect(cx - j, cy - j, i, i, color2);
		Adafruit_GFX_fillRect(cx - j, cy - j, i, i, color1);
	}
}

void testText() {
	char* hello = "Hello World!";
	char* num = "1234.56";
	char* hex = "DEADBEEF";
	char* groop = "Groop";

	Adafruit_GFX_fillScreen(0);
	
	//Print hello world with white color, black background, default size
	for(int i = 0; i < 13; i++)
		Adafruit_GFX_drawChar(i * 6, 0, *(hello+i), 0xFFFF, 0x0000, 1);

	for(int i = 0; i < 7; i++)
		Adafruit_GFX_drawChar(i * 12, 9, *(num+i), 0xFFE0, 0x0000, 2);

	for(int i = 0; i < 10; i++)
		Adafruit_GFX_drawChar(i * 18, 27, *(hex+i), 0xF800, 0x0000, 3);
	
	for(int i = 0; i < 5; i++)
		Adafruit_GFX_drawChar(i * 30, 63, *(groop+i), 0x07E0, 0x0000, 5);
}

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
	testTriangles();
	testFilledRects(0xFFE0, 0xF81F);
	testText();
	
	
    // Enter main loop.
    for (;;)
    {
		__WFE();
    }
}


/**
 * @}
 */

#include "TouchScreen.h"

//Store the ADC value here
volatile uint16_t result;

uint16_t getADCValue(nrf_saadc_input_t in) {

	//Set input for ADC
	NRF_SAADC->CH[0].PSELP = in << SAADC_CH_PSELP_PSELP_Pos;

	//Start the SAADC and wait for the started event.
	NRF_SAADC->TASKS_START = 1;
	while (NRF_SAADC->EVENTS_STARTED == 0);
	NRF_SAADC->EVENTS_STARTED = 0;

	//Do a SAADC sample, will put the result in the configured RAM buffer.
	NRF_SAADC->TASKS_SAMPLE = 1;
	while (NRF_SAADC->EVENTS_END == 0);
	NRF_SAADC->EVENTS_END = 0;

	//Stop the SAADC
	NRF_SAADC->TASKS_STOP = 1;
	while (NRF_SAADC->EVENTS_STOPPED == 0);
	NRF_SAADC->EVENTS_STOPPED = 0;

	return result;
}

/**@brief Maps the raw value which can be from 0 to 350 to 0 to 320.
   Modified version of the Arduino map function.
 */
int16_t mapX(int raw) {
	float val = (raw - X_OFFSET) * (X_MAX) / (RAW_X_MAX);
	return (int16_t) val;
}

/**@brief Maps the raw value which can be from 0 to 290 to 0 to 240.
 */
int16_t mapY(int raw) {
	float val = (raw - Y_OFFSET) * (Y_MAX) / (RAW_Y_MAX);
	return (int16_t) val;
}

/**@brief Read the latest X position from the analog pin
 */
uint16_t readTouchX(void) {
	//Init pins
	nrf_gpio_pin_dir_set(Y_PLUS, NRF_GPIO_PIN_DIR_INPUT);
	nrf_gpio_pin_dir_set(Y_MINUS, NRF_GPIO_PIN_DIR_INPUT);

	nrf_gpio_pin_dir_set(X_PLUS, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(X_MINUS, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_write(X_PLUS, 1);
	nrf_gpio_pin_write(X_MINUS, 0);

	//Use ADC on Y+ pin
	return mapX(1023 - getADCValue(NRF_SAADC_INPUT_AIN4));
}

/**@brief Read the latest Y position from the analog pin
 */
uint16_t readTouchY(void) {
	//Init pins
	nrf_gpio_pin_dir_set(X_PLUS, NRF_GPIO_PIN_DIR_INPUT);
	nrf_gpio_pin_dir_set(X_MINUS, NRF_GPIO_PIN_DIR_INPUT);

	nrf_gpio_pin_dir_set(Y_PLUS, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(Y_MINUS, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_write(Y_PLUS, 1);
	nrf_gpio_pin_write(Y_MINUS, 0);

	//Use ADC on X- pin
	return mapY(1023 - getADCValue(NRF_SAADC_INPUT_AIN5));
}

/**@brief Read the current pressure from the touch screen.
 */
uint16_t pressure(void) {
	//Set X+ low, Y- high, X- and Y+ high impendence
	nrf_gpio_pin_dir_set(X_PLUS, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_write(X_PLUS, 0);
	nrf_gpio_pin_dir_set(X_PLUS, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_write(Y_MINUS, 1);

	nrf_gpio_pin_write(X_MINUS, 0);
	nrf_gpio_pin_dir_set(Y_MINUS, NRF_GPIO_PIN_DIR_INPUT);
	nrf_gpio_pin_write(Y_PLUS, 0);
	nrf_gpio_pin_dir_set(Y_MINUS, NRF_GPIO_PIN_DIR_INPUT);

	//Simple algorithm for calculating pressure. Returns 750-775 when no presses, > 850 otherwise
	return 1023 - getADCValue(NRF_SAADC_INPUT_AIN4) + getADCValue(NRF_SAADC_INPUT_AIN5);
}

/**@brief Init the ADC. Set channel, 10-bit resolution, and result pointer.
 */
void saadc_init(void) {
	//Start HFCLK from crystal oscillator, this will give the SAADC higher accuracy
	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

	//Config channel 0, singled-ended channel, Internal reference (0.6V) and 1/6 gain.
	NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) |
	                          (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |
							  (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
							  (SAADC_CH_CONFIG_RESN_Bypass << SAADC_CH_CONFIG_RESN_Pos) |
							  (SAADC_CH_CONFIG_RESP_Bypass << SAADC_CH_CONFIG_RESP_Pos) |
							  (SAADC_CH_CONFIG_TACQ_3us << SAADC_CH_CONFIG_TACQ_Pos);

	//Configure positive pinlater. Configure no connection for negative pin
	NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos;

	NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_10bit << SAADC_RESOLUTION_VAL_Pos;

	//Configure result to be put in RAM at the location of "result" variable.
	NRF_SAADC->RESULT.MAXCNT = 1;
	NRF_SAADC->RESULT.PTR = (uint32_t) &result;

	//No automatic sampling, will trigger with TASKS_SAMPLE.
	NRF_SAADC->SAMPLERATE = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos;

	//Enable SAADC (would capture analog pins if they were used in CH[0].PSELP)
	NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;

	//Calibrate the SAADC (only needs to be done once in a while)
	NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
	while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
	NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
	while (NRF_SAADC->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos));
}

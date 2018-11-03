// Blink app
//
// Blinks the LEDs on Buckler

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"

// #include "buckler.h"

// This value is empirically determined, taken from the Arduino library
const int CS_Timeout_Millis = 2000 * 310;

#define SEND_PIN 6
#define RECV_PIN 7

// ----------------------------
// Attempts to sense one touch output event. 
// If no event is detected, -1 is returned
// ----------------------------
int SenseOneCycle() {
	int total = 0;
	// Turn off the send pin, let the recv pin drain to 0v
	// This is accomplished by setting it from input->output->input
	nrf_gpio_pin_clear(SEND_PIN);
	nrf_gpio_cfg_input(RECV_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_output(RECV_PIN);
	nrf_gpio_pin_clear(RECV_PIN);
	nrf_delay_ms(10);
	nrf_gpio_cfg_input(7, NRF_GPIO_PIN_NOPULL);

	// Turn on SEND signal, wait for RECV pin to charge up to 5v
	nrf_gpio_pin_set(SEND_PIN);
	while(!nrf_gpio_pin_read(RECV_PIN) && total < CS_Timeout_Millis) {
		// takes about 10 ticks to charge up
		total++;
	}
	if (total >= CS_Timeout_Millis) {
		return -1;
	}

	// This charges up RECV pin a little bit more (above threshold
	// determined by the above loop)
	nrf_gpio_pin_set(RECV_PIN);
	nrf_gpio_cfg_output(RECV_PIN);
	nrf_gpio_pin_set(RECV_PIN);
	nrf_gpio_cfg_input(RECV_PIN, NRF_GPIO_PIN_NOPULL);
	// Turn off send signal
	nrf_gpio_pin_clear(SEND_PIN);

	// Wait for RECV pin to drain (if user is touching fabric)
	// or wait for timeout
	while (nrf_gpio_pin_read(7) && total < CS_Timeout_Millis) {
		total++;
	}
	// Return value changes based on how much of the fabric is touched
	return (total >= CS_Timeout_Millis) ? -1 : total;
}

// This value is determined by the size of the conductive fabric 
// determined experimentally
uint32_t touchThreshold = 1000;

// ----------------------------
// Returns true if detected touch event is above the set threshold
// ----------------------------
bool SenseInput() {
	int value = SenseOneCycle();
	if (value == -1) { return false; }
	return value > touchThreshold;
}

// just so we can keep track of outputs
int counter = 0;

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  nrf_gpio_cfg_output(6);
  nrf_gpio_cfg_input(7, NRF_GPIO_PIN_NOPULL);

  // loop forever
  while (1) {
	counter++;
	printf("%d: %s\n", counter, SenseInput() ? "true" : "false");
	//printf("%d: %ld\n", counter, SenseOneCycle());

  }
}


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

uint32_t SenseOneCycle() {
	// This DOesn't Work!
	// nrf_gpio_pin_set(pin)
	// nrf_gpio_pin_clear(pin)
	nrf_gpio_pin_clear(6);
	nrf_gpio_cfg_input(7, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_output(7);
	nrf_delay_ms(10);
	nrf_gpio_cfg_input(7, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_pin_set(6);
	nrf_delay_ms(10);
	//nrf_gpio_pin_clear(6);
	nrf_delay_ms(10);
	return nrf_gpio_pin_read(7);
}

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  nrf_gpio_cfg_output(6);
  nrf_gpio_cfg_input(7, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_pin_set(6);

  // loop forever
  while (1) {
	printf("%ld\n", SenseOneCycle());
	//printf("%ld\n", nrf_gpio_pin_read(7));
  }
}


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
#include "serial_output.h"

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;
    // initialize serial library
  error_code = serial_init();
  APP_ERROR_CHECK(error_code);

  int i = 0;

  // loop forever
  while (1) {
    error_code = serial_send(i);
    i++;

    APP_ERROR_CHECK(error_code);
    nrf_delay_ms(10);
  }
}


// Button and Switch app
//
// Uses a button and a switch to control LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrfx_gpiote.h"
#include "mcp23017.h"

// I2C manager

#define SCL_PIN 29
#define SDA_PIN 28

#define READ_BIT(v, b) ((v >> b) & 1)
void print_binary(uint8_t value) {
    printf("%d%d%d%d%d%d%d%d\n", 
            READ_BIT(value, 7), 
            READ_BIT(value, 6), 
            READ_BIT(value, 5), 
            READ_BIT(value, 4), 
            READ_BIT(value, 3), 
            READ_BIT(value, 2), 
            READ_BIT(value, 1), 
            READ_BIT(value, 0));
}

// This value is empirically determined, taken from the Arduino library
const int CS_Timeout_Millis = 10000;

static int dischargeTimes[8] = {0};

// ----------------------------
// Attempts to sense one touch output event. 
// If no event is detected, -1 is returned
// ----------------------------
int SenseOneCycle(int i) {
	int total = 0;
    dischargeTimes[i] = 0;

    mcp_set_output(sideA);
    mcp_write_values(sideA, ~(1 << i));
    // wait for the pin to full lose charge. There might be a faster
    // way to do this
	nrf_delay_ms(3);
    mcp_set_input(sideA);

    // Notice the logical NOT, This is because we are looking for active high
    uint8_t value = ~mcp_read_values(sideA);
	while ((value & (1 << i)) != 0 && total < CS_Timeout_Millis) {
		total++;
        dischargeTimes[i] += 1;
        value = ~mcp_read_values(sideA);
	}
	// Return value changes based on how much of the fabric is touched
	return (total >= CS_Timeout_Millis) ? -1 : total;
}

void SenseAll() {
    for (int i = 0; i < 5; ++i) {
        SenseOneCycle(i);
    }
}

// This value is determined by the size of the conductive fabric 
// determined experimentally
int touchThreshold = 1000;

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized\n");

  error_code = mcp_init(SDA_PIN, SCL_PIN, NRF_TWIM_FREQ_400K);
  APP_ERROR_CHECK(error_code);

  //set_output(sideA);
  mcp_set_input(sideA);
  nrf_delay_ms(50);
  mcp_set_pull(0x00);
  int counter = 0;


  // initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  printf("Started\n");
  while (1) {
	counter++;
	// printf("%d: %s\n", counter, SenseInput() ? "true" : "false");
    // if (SenseOneCycle() == -1) {
    //     printf("Timeout\n");
    // } else {
        SenseAll();
        printf("%d: %3d %3d %3d %3d %3d\n", counter, 
                dischargeTimes[0], 
                dischargeTimes[1],
                dischargeTimes[2],
                dischargeTimes[3],
                dischargeTimes[4]);
    // }
    nrf_delay_ms(10);
  }
}


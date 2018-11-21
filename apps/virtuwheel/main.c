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
#include "serial_output.h"

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

static float dischargeTimes[36] = {0};

// ----------------------------
// Attempts to sense one touch output event. 
// If no event is detected, -1 is returned
// ----------------------------
int SenseOneCycle() {
	int total = 0;
    dischargeTimes[0] = 0;
    dischargeTimes[1] = 0;
    dischargeTimes[2] = 0;
    dischargeTimes[3] = 0;
    dischargeTimes[4] = 0;
    dischargeTimes[5] = 0;
    dischargeTimes[6] = 0;
    dischargeTimes[7] = 0;

    mcp_set_output(sideA);
    mcp_write_values(sideA, 0);
    // wait for the pin to full lose charge. There might be a faster
    // way to do this
	nrf_delay_ms(50);
    mcp_set_input(sideA);

    // Notice the logical NOT, This is because we are looking for active high
    uint8_t value = ~mcp_read_values(sideA);
	while (value != 0 && total < CS_Timeout_Millis) {
		total++;
        dischargeTimes[0] += value&1;
        dischargeTimes[1] += (value>>1)&1;
        dischargeTimes[2] += (value>>2)&1;
        dischargeTimes[3] += (value>>3)&1;
        dischargeTimes[4] += (value>>4)&1;
        dischargeTimes[5] += (value>>5)&1;
        dischargeTimes[6] += (value>>6)&1;
        dischargeTimes[7] += (value>>7)&1;
        value = ~mcp_read_values(sideA);
	}
	// Return value changes based on how much of the fabric is touched
	return (total >= CS_Timeout_Millis) ? -1 : total;
}

// This value is determined by the size of the conductive fabric 
// determined experimentally
int touchThreshold = 1000;

// ----------------------------
// Returns true if detected touch event is above the set threshold
// ----------------------------
bool SenseInput() {
	int value = SenseOneCycle();
	if (value == -1) { return false; }
	return value > touchThreshold;
}

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

  serial_init();

  // initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  printf("Started\n");
  while (1) {
	counter++;
	// printf("%d: %s\n", counter, SenseInput() ? "true" : "false");
    if (SenseOneCycle() == -1) {
        printf("Timeout\n");
    } else {
        // printf("%d: %5.2f %5.2f %5.2f\n", counter, 
        //         dischargeTimes[0], 
        //         dischargeTimes[1],
        //         dischargeTimes[2]);
        serial_send(dischargeTimes);
    }
    nrf_delay_ms(10);
  }
}


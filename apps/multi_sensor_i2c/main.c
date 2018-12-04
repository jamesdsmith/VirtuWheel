// Button and Switch app
//
// Uses a button and a switch to control LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "serial_output.h"
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

static int dischargeTimes[32] = {0};
static int touchThreshold = 2;

// ----------------------------
// Attempts to sense one touch output event. 
// If no event is detected, -1 is returned
// ----------------------------
int SenseOne(int i, uint8_t side) {
	int total = 0;
    // this relies on the values defined in mcp23017.h
    int idx = side * 8 + i;
    dischargeTimes[idx] = 0;

    mcp_set_output(side);
    mcp_write_values(side, ~(1 << i));
    // wait for the pin to full lose charge. There might be a faster
    // way to do this
	nrf_delay_ms(3);
    mcp_set_input(side);

    // Notice the logical NOT, This is because we are looking for active high
    uint8_t value = ~mcp_read_values(side);
	while ((value & (1 << i)) != 0 && total < CS_Timeout_Millis) {
		total++;
        dischargeTimes[idx] += 1;
        value = ~mcp_read_values(side);
	}
  dischargeTimes[idx] /= touchThreshold;
	// Return value changes based on how much of the fabric is touched
	return (total >= CS_Timeout_Millis) ? -1 : total;
}

void SenseAll() {
    uint8_t sides[4] = {sideA, sideB, sideC, sideD};
    for (int side_idx = 0; side_idx < 4; ++side_idx) {
        for (int i = 0; i < 8; ++i) {
            SenseOne(i, sides[side_idx]);
        }
    }
}

// This value is determined by the size of the conductive fabric 
// determined experimentally


int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized\n");

  error_code = mcp_init(SDA_PIN, SCL_PIN, NRF_TWIM_FREQ_400K);
  APP_ERROR_CHECK(error_code);
  printf("MCP init\n");
  error_code = serial_init();
  APP_ERROR_CHECK(error_code);
  printf("Serial init\n");
  mcp_set_input(sideA);
  printf("SideA\n");
  mcp_set_input(sideB);
  printf("SideB\n");
  mcp_set_input(sideC);
  printf("SideC\n");
  mcp_set_input(sideD);
  printf("SideD\n");
  nrf_delay_ms(50);
  // mcp_set_pull(0x00);
  int counter = 0;
  printf("Sides set\n");

  // initialize GPIO driver
  // if (!nrfx_gpiote_is_init()) {
  //   error_code = nrfx_gpiote_init();
  // }
  // APP_ERROR_CHECK(error_code);
  // printf("gpio_init\n");



  printf("Started\n");
  while (1) {
	  counter++;
    SenseAll();
    serial_send(dischargeTimes);
    nrf_delay_ms(10);
  }
}


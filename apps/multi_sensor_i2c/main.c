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
#include "nrf_twi_mngr.h"
#include "nrfx_gpiote.h"

#include "buckler.h"
#include "mpu9250.h"

// I2C manager
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);
#define i2c_addr 0x20
#define sideA 0x00
#define sideB 0x01
#define reg_offset 0x12
#define SCL_PIN 29
#define SDA_PIN 28

#define SEND_PIN 6
#define RECV_PIN 7

static const nrf_twi_mngr_t* i2c_manager = NULL;
void set_output(uint8_t A)
{
  uint8_t buff[2] = {A, 0x00};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
  // nrf_delay_ms(50);
}

void set_pull(uint8_t value)
{
  uint8_t buff[2] = {0x06, value};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}

void set_input(uint8_t A)
{
  uint8_t buff[2] = {A, 0xFF};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}
void write_values(uint8_t side, uint8_t value)
{
  uint8_t buff[2] = {side+ reg_offset, value};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}
uint8_t read_values(uint8_t side) {
  uint8_t rx_buf = 0;
  uint8_t reg_addr = side + reg_offset;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0),
  };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
  APP_ERROR_CHECK(error_code);
  return rx_buf;
}

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

    set_output(sideA);
    write_values(sideA, 0);
    // wait for the pin to full lose charge. There might be a faster
    // way to do this
	nrf_delay_ms(50);
    set_input(sideA);

    // Notice the logical NOT, This is because we are looking for active high
    uint8_t value = ~read_values(sideA);
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
        value = ~read_values(sideA);
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

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized\n");

  // initialize i2c master (two wire interface)
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = SCL_PIN;
  i2c_config.sda = SDA_PIN;
  i2c_config.frequency = NRF_TWIM_FREQ_400K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);


  i2c_manager = &twi_mngr_instance;
  //set_output(sideA);
  set_input(sideA);
  nrf_delay_ms(50);
  set_pull(0x00);
  int counter = 0;


  // initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  nrf_gpio_cfg_output(SEND_PIN);

  printf("Started\n");
  while (1) {
	counter++;
	// printf("%d: %s\n", counter, SenseInput() ? "true" : "false");
    if (SenseOneCycle() == -1) {
        printf("Timeout\n");
    } else {
        printf("%d: %3d %3d %3d\n", counter, 
                dischargeTimes[0], 
                dischargeTimes[1],
                dischargeTimes[2]);
    }
    nrf_delay_ms(10);
  }
}


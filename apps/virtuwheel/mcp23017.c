#include "mcp23017.h"

#define i2c_addr 0x20
#define reg_offset 0x12

static const nrf_twi_mngr_t* i2c_manager = NULL;
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

ret_code_t mcp_init(int sda_pin, int scl_pin, int freq) {
  // initialize i2c master (two wire interface)
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = scl_pin;
  i2c_config.sda = sda_pin;
  i2c_config.frequency = freq;
  ret_code_t error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  i2c_manager = &twi_mngr_instance;
  return error_code;
}

void mcp_set_output(uint8_t A)
{
  uint8_t buff[2] = {A, 0x00};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
  // nrf_delay_ms(50);
}

void mcp_set_pull(uint8_t value) {
  uint8_t buff[2] = {0x06, value};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}

void mcp_set_input(uint8_t A) {
  uint8_t buff[2] = {A, 0xFF};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}

void mcp_write_values(uint8_t side, uint8_t value) {
  uint8_t buff[2] = {side+ reg_offset, value};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buff, 2, 0),
   };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}

uint8_t mcp_read_values(uint8_t side) {
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


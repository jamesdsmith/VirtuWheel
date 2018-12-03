#ifndef _MCP23017_H_
#define _MCP23017_H_

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_twi_mngr.h"

// chip 0, sides 1 and 2
#define sideA 0x00
#define sideB 0x01
// chip 1, sides 1 and 2
#define sideC 0x02
#define sideD 0x03

ret_code_t mcp_init(int sda_pin, int scl_pin, int freq);
void mcp_set_output(uint8_t A);
// void mcp_set_pull(uint8_t value);
void mcp_set_input(uint8_t A);
void mcp_write_values(uint8_t side, uint8_t value);
uint8_t mcp_read_values(uint8_t side);

#endif

#include "nrf_serial.h"
#include "/home/student/buckler/software/nrf5x-base/boards/nrf52dk/nrf52dk.h"

#define VERSION 1

ret_code_t serial_init();
ret_code_t serial_send(int value);
#include "serial_output.h"

// Serial configuration
// create a uart instance for UARTE0
NRF_SERIAL_UART_DEF(serial_uart_instance, 0);
// configuration for uart, RX & TX pin, empty RTS and CTS pins,
//  flow control disabled, no parity bit, 115200 baud, default priority
NRF_SERIAL_DRV_UART_CONFIG_DEF(serial_uart_config, NRF52DK_UART_RXD, NRF52DK_UART_TXD, 0, 0,
      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED, NRF_UART_BAUDRATE_115200, UART_DEFAULT_CONFIG_IRQ_PRIORITY);
// create serial queues for commands, tx length 32, rx length 32
NRF_SERIAL_QUEUES_DEF(serial_queues, 32, 32);
// create serial buffers for data, tx size 100 bytes, rx size 100 bytes
NRF_SERIAL_BUFFERS_DEF(serial_buffers, 100, 100);
// create a configuration using DMA with queues for commands and buffers for data storage
// both handlers are set to NULL as we do not need to support them
NRF_SERIAL_CONFIG_DEF(serial_config, NRF_SERIAL_MODE_DMA, &serial_queues, &serial_buffers, NULL, NULL);

ret_code_t serial_init() {
	return nrf_serial_init(&serial_uart_instance, &serial_uart_config, &serial_config);
}

ret_code_t serial_send(int value) {
	char buffer[1024];
	sprintf(buffer, "{ \"version\": %d, \"touch_points\": [%d] }\n", VERSION, value);

	// static char tx_message[] = "UART Initialized!\n";
	// NRF_SERIAL_MAX_TIMEOUT
	return nrf_serial_write(&serial_uart_instance, buffer, strlen(buffer), NULL, NRF_SERIAL_MAX_TIMEOUT);
}
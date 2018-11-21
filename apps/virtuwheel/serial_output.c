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

#define ALL_VALUES(array) \
    array[0], array[1], array[2], array[3], \
    array[4], array[5], array[6], array[7], \
    array[8], array[9], array[10], array[11], \
    array[12], array[13], array[14], array[15], \
    array[16], array[17], array[18], array[19], \
    array[20], array[21], array[22], array[23], \
    array[24], array[25], array[26], array[27], \
    array[28], array[29], array[30], array[31], \
    array[32], array[33], array[34], array[35]

#define VALUE_FORMAT \
    "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f"

// #define ALL_VALUES(array) \
//     array[0], array[1], array[2], array[3]

// #define VALUE_FORMAT \
//     "%.4f, %.4f, %.4f, %.4f"

ret_code_t serial_init() {
	return nrf_serial_init(&serial_uart_instance, &serial_uart_config, &serial_config);
}

ret_code_t serial_send(float values[]) {
    // message length: 37 + float numbers
    // float numbers = 10*36 = 360
    // 397 total chars, 512 is plenty

	char buffer[512];
	// sprintf(buffer, "{ \"version\": %d, \"touch_points\": [%.4f] }\n", VERSION, values[0]);
	sprintf(buffer, "{ \"version\": %d, \"touch_points\": ["VALUE_FORMAT"] }\n", VERSION, ALL_VALUES(values));
 
    // Debug print the message to rtt
    // printf("%s", buffer);

	return nrf_serial_write(&serial_uart_instance, buffer, strlen(buffer), NULL, NRF_SERIAL_MAX_TIMEOUT);
}

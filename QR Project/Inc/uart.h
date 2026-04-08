#ifndef UART_H
#define UART_H

#include <stdint.h>

// Minimal polling UART2 API (register-level).
void UART_Init(void);
void uart_tx_byte(uint8_t b);
uint8_t uart_rx_byte(void);
void uart_wait_tc(void);
void uart_tx_string(const char *s);

#endif // UART_H

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

// Initialize SPI1 in master, 8-bit, mode 0, software NSS
void SPI1_Init(void);

// Blocking transmit of one byte over SPI1
void SPI1_TransmitByte(uint8_t data);

#endif // SPI_H

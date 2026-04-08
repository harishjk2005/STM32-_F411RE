#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "stm32f4xx.h" 

void UART2_Init(void);
void UART2_WriteChar(char ch);
char UART2_ReadChar(void);
void UART2_WriteString(const char *str);

#endif

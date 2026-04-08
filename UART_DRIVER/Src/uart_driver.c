#include "uart_driver.h"

void UART2_Init(void) {
    // Enable GPIOA and USART2 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Set PA2 and PA3 to alternate function mode (AF7 for USART2)
    GPIOA->MODER &= ~((3 << (2 * 2)) | (3 << (3 * 2)));
    GPIOA->MODER |=  (2 << (2 * 2)) | (2 << (3 * 2)); // Alternate function

    GPIOA->AFR[0] &= ~((0xF << (4 * 2)) | (0xF << (4 * 3))); // Clear AFR bits
    GPIOA->AFR[0] |=  (7 << (4 * 2)) | (7 << (4 * 3));       // Set AF7

    // Assuming APB1 clock = 42 MHz (check in debugger), for 9600 baud:
    USART2->BRR = 4375;

    // Enable TX, RX, and USART2
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void UART2_WriteChar(char ch) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = ch;
}

char UART2_ReadChar(void) {
    while (!(USART2->SR & USART_SR_RXNE));
    return USART2->DR;
}

void UART2_WriteString(const char *str) {
    while (*str) {
        UART2_WriteChar(*str++);
    }
}

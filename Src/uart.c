#include <stdint.h>
#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840)
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
// GPIOA base
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000)
#define GPIOA_OSPEEDR (*(volatile uint32_t *)0x40020008)
#define GPIOA_PUPDR  (*(volatile uint32_t *)0x4002000C)
#define GPIOA_OTYPER (*(volatile uint32_t *)0x40020004)
#define GPIOA_AFRL   (*(volatile uint32_t *)0x40020020)
// USART2 base (correct address for APB1)
#define USART_SR  (*(volatile uint32_t *)0x40004400)
#define USART_DR  (*(volatile uint32_t *)0x40004404)
#define USART_BRR (*(volatile uint32_t *)0x40004408)
#define USART_CR1 (*(volatile uint32_t *)0x4000440C)
#define USART_CR2 (*(volatile uint32_t *)0x40004410)
    
// Enable GPIOA and USART2 clocks, then set PA2/PA3 to AF7, very high speed, pull-up, push-pull.
void GPIOA_INIT(void){
    RCC_AHB1ENR |= 0X01;
    RCC_APB1ENR |= 1<<17;
    GPIOA_MODER &= ~(0XF<<4);
    GPIOA_MODER |= 0XA << 4;
    GPIOA_OSPEEDR |= 0XF << 4;
    GPIOA_PUPDR &= ~(0XF << 4);
    GPIOA_PUPDR |= 0X5 << 4;
    GPIOA_OTYPER &= ~(0X3 << 2);
    GPIOA_AFRL &= ~(0XFF<<8);
    GPIOA_AFRL|= 0X77 << 8;
}
void UART_Init(void){
    GPIOA_INIT();
    USART_CR1 &= ~(1<<13); // DISABLE USART
    USART_BRR = 0x008B ; // 115200 baud rate
    USART_CR1 &= ~(1<<12); // 8 data bits
    USART_CR1 &= ~(1<<10); // parity disabled
    USART_CR2 &= ~(0X3 << 12); // 1 stop bit
    USART_CR1 |= 1<<13; // enable USART   
    USART_CR1 |= 1<<3; // enable receiver
    USART_CR1 |= 1<<2; // enable transmitter
}


void uart_tx_byte(uint8_t b){
    while(!(USART_SR & (1<<7))){} // TXE
    USART_DR = b;
}

// Blocking receive one byte (wait RXNE, read DR).
uint8_t uart_rx_byte(void){
    while(!(USART_SR & (1<<5))){} // RXNE
    return (uint8_t)USART_DR;
}

// Wait for transmission complete (TC) after the final byte of a frame.
void uart_wait_tc(void){
    while(!(USART_SR & (1<<6))){} // TC
}

// Send a null-terminated string via polling.
void uart_tx_string(const char *s){
    while(*s){
        uart_tx_byte((uint8_t)*s++);
    }
    uart_wait_tc();
}

    





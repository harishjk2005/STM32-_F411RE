#include <stdint.h>
#include "spi.h"
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
#define RCC_APB2ENR (*(volatile uint32_t *)0x40023844)
// GPIOA 
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000)
#define GPIOA_OSPEEDR (*(volatile uint32_t *)0x40020008)
#define GPIOA_PUPDR  (*(volatile uint32_t *)0x4002000C)
#define GPIOA_OTYPER (*(volatile uint32_t *)0x40020004)
#define GPIOA_AFRL   (*(volatile uint32_t *)0x40020020)

//SPI1
#define SPI1_CR1 (*(volatile uint32_t *) 0x40013000)
#define SPI1_CR2 (*(volatile uint32_t *) 0x40013004)
#define SPI1_SR  (*(volatile uint32_t *) 0x40013008)
#define SPI1_DR  (*(volatile uint32_t *) 0x4001300C)

void SPI1_Init(void){
    RCC_AHB1ENR |= 0X01; // GPIOA clock
    RCC_APB2ENR |= 1<<12; // SPI1 clock

    // SPI1 pins: PA5=SCK, PA6=MISO, PA7=MOSI (AF5)
    GPIOA_MODER &= ~(0x3F << 10);  // clear PA5,6,7
    GPIOA_MODER |=  (0x2A << 10);  // AF mode (10 10 10)
    GPIOA_OSPEEDR |= 0X3F<<10; // Very high speed   
    GPIOA_PUPDR &= ~(0X3F << 10);
    // GPIOA_PUPDR |= 0X15 << 10; // Pull-up for PA5,7
    GPIOA_OTYPER &= ~(0X7 << 5); // Push-pull
    GPIOA_AFRL &= ~(0xFFF<<20);
    GPIOA_AFRL |=  0x555<<20; // AF5 for PA5-7

    // Reset SPI registers
    SPI1_CR1 = 0;
    SPI1_CR2 = 0;

    // SPI1: master, 8-bit, CPOL=0, CPHA=0 (mode 0), software NSS, baud = fPCLK/2 (BR=0b000)
    SPI1_CR1 |= (1<<2);          // MSTR=1 (master mode)
    SPI1_CR1 |= (1<<9) | (1<<8); // SSM=1, SSI=1 (manage NSS in software)
    SPI1_CR1 &= ~(1<<11);        // DFF=0 (8-bit)
    SPI1_CR1 &= ~(1<<1);         // CPOL=0
    SPI1_CR1 &= ~(1<<0);         // CPHA=0
    SPI1_CR1 &= ~(0b111 << 3);   // BR = 0b000 -> fPCLK/2 (~8 MHz at 16 MHz PCLK2)
    SPI1_CR1 |= (1<<6);          // SPE enable
}

// Blocking transmit one byte over SPI1
void SPI1_TransmitByte(uint8_t data){
    while(!(SPI1_SR & (1<<1))){}
    *(volatile uint8_t*)&SPI1_DR = data;
    while(SPI1_SR & (1<<7)){}
}

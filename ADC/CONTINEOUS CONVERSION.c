#include "stm32f4xx.h"
int main(){

	RCC->CR |=(1<<0); //enable clock hsi
	RCC->APB2ENR |= (1<<8);//enable adc clock
	RCC->AHB1ENR |=(1<<0);//enable gpio clock
	GPIOA->MODER |= (3 << 0);//setting pa0 as analog
	ADC1->CR2 &= ~(1 << 0); // Disable ADC before configuration
	ADC1->SQR3 |= (0 << 0); // Select channel 0 (PA0)
	ADC1->SQR1 = 0;
	ADC1->SMPR2 |=(7<<0) ;//SETTING SAMPLING 480
	ADC1->CR2 |= ( 1 << 0);
	GPIOA->MODER |= (1 << 10);  // Set PA5 as output (01)
    GPIOA->OTYPER &= ~(1 << 5); // Push-pull mode
    GPIOA->OSPEEDR |= (2 << 10); // High speed

    uint16_t adc_val = ADC1->DR;
    while(1){
    	ADC1->CR2 |=(0x1 << 30);
    	   	while(!(ADC1->SR & (0x1 << 1))){}
    		if(adc_val > 1500){
    			GPIOA->ODR |= (1 << 5 );
    		}
    		else{
    			GPIOA->ODR &= ~(1 << 5);
    		}
    		for (volatile uint32_t i = 0; i < 100000; i++);
    	}
    }

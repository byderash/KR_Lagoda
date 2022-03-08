
#ifndef USART_DMA_H_
#define USART_DMA_H_

#include "stm32f0xx.h"
#include "global.h"


void USART1_DMA_Init();
void USART1_GPIO_Init();
void usart_start();
void send_temp(uint16_t temperature);
void DMA_USART(uint8_t size);
void DMA_USART_EN(uint8_t size);

#endif /* USART_DMA_H_ */

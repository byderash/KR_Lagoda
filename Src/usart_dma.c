#include "usart_dma.h"


uint32_t data_for_usart[16];
extern uint16_t mode;

void USART1_GPIO_Init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
	GPIOA->AFR[1] |= (0x01 << GPIO_AFRH_AFSEL9_Pos) | (0x01 << GPIO_AFRH_AFSEL10_Pos); /* (4) */
}

void USART1_DMA_Init(){
	USART1_GPIO_Init();

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	USART1->BRR |= SystemCoreClock/BAUD; //выбираем скорость

	USART1->CR3 |= USART_CR3_OVRDIS;
	USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;

	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; //разрешаем отправку и прием
	while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
	USART1->ICR |= USART_ICR_TCCF; //clear TC flag

	USART1->CR1 |= USART_CR1_UE;
	DMA_USART(2);
}

// на вход температура
void send_temp(uint16_t temperature){
	if(mode != temp_ready){
		return;
	}
	uint32_t data = (temperature & 0xFF00)>>8;
	data_for_usart[0]=(uint32_t)(temperature & 0x00FF);
	data_for_usart[1]=(uint32_t)(data);
	DMA_USART_EN(2);
}


void DMA_USART(uint8_t size){
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR));
	DMA1_Channel2->CMAR = (uint32_t)(&data_for_usart[0]);
	DMA1_Channel2->CNDTR = size;
	DMA1_Channel2->CCR |= DMA_CCR_PL_0;
	DMA1_Channel2->CCR |= DMA_CCR_MSIZE_1;
	DMA1_Channel2->CCR |= DMA_CCR_PSIZE_1;
	DMA1_Channel2->CCR |= DMA_CCR_MINC;
	DMA1_Channel2->CCR |= DMA_CCR_DIR;//read from memory

	SYSCFG->CFGR1 &= (~SYSCFG_CFGR1_USART1TX_DMA_RMP);
	DMA1_Channel2->CCR |= DMA_CCR_TCIE;
	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 5);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

// включение передачт по усрат, на вход также массив и его размер
void DMA_USART_EN(uint8_t size){
	DMA1_Channel2->CCR &=~DMA_CCR_EN;
	DMA1_Channel2->CMAR = (uint32_t)(&data_for_usart[0]);
	DMA1_Channel2->CNDTR = size;
	DMA1_Channel2->CCR |=  DMA_CCR_EN;
}


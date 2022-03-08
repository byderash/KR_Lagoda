#include "spi_dma.h"

uint8_t spi_buffer_tx[BUFFER_SPI_SIZE];

extern uint16_t mode;
extern uint16_t data_MAX;
extern void delay(uint32_t tick);

uint16_t status_dma_tx = 1;

void DMA1_CH2_3_DMA2_CH1_2_IRQHandler(){
	if (DMA1->ISR & DMA_ISR_TCIF3) {
		DMA1->IFCR |= DMA_IFCR_CTCIF3;
		status_dma_tx = 1;
		SET_CS();
		return;
	}
	if (DMA1->ISR & DMA_ISR_TCIF2){
		DMA1->IFCR |= DMA_IFCR_CTCIF2;
		mode = ready_transmit;
		return;
	}
}


void dma_tx_init(){
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel3->CMAR = (uint32_t)(&spi_buffer_tx[0]);
	DMA1_Channel3->CPAR = (uint32_t)(&(SPI1->DR));
	DMA1_Channel3->CCR |= DMA_CCR_MINC;
	DMA1_Channel3->CCR |= DMA_CCR_DIR;
	DMA1_Channel3->CCR |= DMA_CCR_TCIE;
	DMA1_Channel3->CCR |= DMA_CCR_PL_1;

	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 8);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}


void SPI2_master_init(){
	GPIOB_SPI2_master_Init();

	RCC->APB1ENR|=RCC_APB1ENR_SPI2EN;

	SPI2->CR1 |= SPI_CR1_SSI;
	SPI2->CR1 |= SPI_CR1_SSM;

	SPI2->CR2 |= SPI_CR2_DS_0|SPI_CR2_DS_1|SPI_CR2_DS_2|SPI_CR2_DS_3; /* size 16 bits */

	SPI2->CR1 &= ~(SPI_CR1_CPOL);
	SPI2->CR1 |= SPI_CR1_CPHA;

	SPI2->CR1 |= SPI_CR1_MSTR;
	SPI2->CR1|= SPI_CR1_BR_2;//f/32

	SPI2->CR1 |= SPI_CR1_SPE; /*(2)*/
}

void GPIOB_SPI2_master_Init(){
	/*PB12 - SPI2_CS
	 * ; PB13 - SPI2_SCK;  PB14 - SPI2_MISO;  PB15 - SPI2_MOSI
	 * AF0*/
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1|GPIO_MODER_MODER15_1;
	GPIOB->MODER |= GPIO_MODER_MODER12_0; //output
	GPIOB->AFR[1]&=~(GPIO_AFRH_AFSEL15|GPIO_AFRH_AFSEL14|GPIO_AFRH_AFSEL13);
	SET_CS_MAX();
	delay(1000);
}


void spi1_master_init(uint8_t am_bits_send) {
	GPIOA_init_spi_TFT();

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	SPI1->CR1 &= ~SPI_CR1_SPE;

	SPI1->CR1 &= ~SPI_CR1_BR;

	SPI1->CR1 |= SPI_CR1_MSTR;	//master
	SPI1->CR1 |= SPI_CR1_SSI;
	SPI1->CR1 |= SPI_CR1_SSM;

	uint16_t DataSize = SPI1->CR2 & ~SPI_CR2_DS;
	DataSize |= (am_bits_send - 1) << 8;
	SPI1->CR2 = DataSize;
	SPI1->CR2 |= SPI_CR2_FRXTH;

	dma_tx_init();
	SPI1->CR2 |= SPI_CR2_TXDMAEN;

	SPI1->CR1 |= SPI_CR1_SPE;
}

void GPIOA_init_spi_TFT(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//PA5 - MOSI, PA7 - SCK , PA6 - MISO
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1| GPIO_MODER_MODER7_1;
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL7| GPIO_AFRL_AFRL6);
	//for CS and other
	GPIOA->MODER |= GPIO_MODER_MODER4_0|GPIO_MODER_MODER2_0|GPIO_MODER_MODER3_0|GPIO_MODER_MODER1_0;
	SET_CS();
	RESET_IDLE();
}

void spi1_setDataSize(uint8_t am_bits_send) {
	SPI1->CR1 &= ~SPI_CR1_SPE;
	uint16_t DataSize = SPI1->CR2 & ~SPI_CR2_DS;
	DataSize |= (am_bits_send - 1) << 8;
	SPI1->CR2 = DataSize;
	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_SendDataDMA_1Byte(uint8_t* data, uint16_t count_byte) {
	while((SPI1->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	status_dma_tx = 0;
	spi1_setDataSize(8);

	for (int i = 0; i < count_byte; i++) {
		spi_buffer_tx[i] = data[i];
	}

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CNDTR = count_byte;
	DMA1_Channel3->CCR |= DMA_CCR_MINC;	//increment memory
	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;	//size 8bits of data and peripheral

	RESET_CS();
	Start_DMA_Send_Data();
}

void spi1_SendDataDMA_2byte(uint16_t *data, uint16_t count_word) {
	while((SPI1->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	status_dma_tx = 0;

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;//16 bits
	DMA1_Channel3->CCR |= DMA_CCR_MINC;
	DMA1_Channel3->CMAR = (uint32_t)(&spi_buffer_tx[0]);
	DMA1_Channel3->CNDTR = count_word;

	spi1_setDataSize(16);

	count_word = count_word << 1;
	for (int i = 0; i < count_word; i += 2, data++) {
		spi_buffer_tx[i] =  0x00ff & (*data >> 8);
		spi_buffer_tx[i+1] = 0x00ff & (*data);
	}

	RESET_CS();
	Start_DMA_Send_Data();
}


void spi1_SendDataDMA_2byteNTimes(uint16_t data, uint32_t count_word) {
	while((SPI1->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	status_dma_tx = 0;
	count_word = count_word<<1;
	//if not mult by 2, CS will up in the middle of sending data


	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;
	DMA1_Channel3->CCR &= ~DMA_CCR_MINC;
	DMA1_Channel3->CMAR = (uint32_t)(&spi_buffer_tx[0]);
	DMA1_Channel3->CNDTR = count_word;

	spi1_setDataSize(16);
	spi_buffer_tx[0] =  0x00ff & (data >> 8);
	spi_buffer_tx[1] = 0x00ff & (data);

	RESET_CS();
	DMA1_Channel3->CCR |= DMA_CCR_EN;
}

void spi2_ReceiveData_2byte() {
	while((SPI2->SR & SPI_SR_TXE)!=SPI_SR_TXE);

	uint16_t data=0;

	RESET_CS_MAX();
	SPI2->DR = data;
	uint16_t i=0;
	while((SPI2->SR & SPI_SR_RXNE) != SPI_SR_RXNE){
		i++;
		if (i>1000)
		{
			mode = Error;
			return;
		}
	}
	data_MAX=SPI2->DR;
	SET_CS_MAX();
}

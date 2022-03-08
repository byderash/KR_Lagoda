#ifndef SPI_DMA_H_
#define SPI_DMA_H_


#include "global.h"
#include "stm32f0xx.h"

#define BUFFER_SPI_SIZE 128

void DMA1_Channel2_3_IRQHandler() ;
void spi1_master_init(uint8_t am_bits_send);
void dma_tx_init();
void spi1_setDataSize(uint8_t am_bits_send);
void spi1_SendDataDMA_1Byte(uint8_t* data, uint16_t count_byte);
void spi1_SendDataDMA_2byte(uint16_t *data, uint16_t count_word);
void spi1_SendDataDMA_2byteNTimes(uint16_t data, uint32_t count_word);
void GPIOA_init_spi_TFT();


void SPI2_master_init();
void GPIOB_SPI2_master_Init();
void spi2_ReceiveData_2byte();

#endif /* SPI_DMA_H_ */

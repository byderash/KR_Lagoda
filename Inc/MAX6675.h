
#ifndef MAX6675_H_
#define MAX6675_H_

#include "stm32f0xx.h"
#include "spi_dma.h"
#include "global.h"

void MAX6675_init();
void MAX6675_get_temp(uint16_t *temp);

#endif /* MAX6675_H_ */

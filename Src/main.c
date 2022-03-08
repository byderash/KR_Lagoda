
#include "main.h"


uint16_t temp=0; //переменная, отвечающая за температуру
uint16_t mode=print_ready; // режим работы  блока контроля

void clock48Mhz_init() { //установка максимальной частоты
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);

	RCC->CFGR |= RCC_CFGR_PLLMUL12;

	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

	RCC->CFGR |= RCC_CFGR_SW_1;	//as PLL

	SystemCoreClockUpdate();
}


int main(void){
	clock48Mhz_init();
#ifdef USART_ON
	USART1_DMA_Init(); // инциализация USART1 (необходима только для исследования)
#else
	TFT_init(); //инциализация экрана
#endif
	MAX6675_init(); // инциализация термопары
	while(1){
		MAX6675_get_temp(&temp); //получение температуры с термопары
#ifdef USART_ON
		send_temp(temp); // передача значения температуры по USART1
#else
		Print(&temp); // вывод температуры на экран
#endif
	}
}

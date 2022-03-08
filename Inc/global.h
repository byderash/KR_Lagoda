#ifndef GLOBAL_H_
#define GLOBAL_H_


#include "stm32f0xx.h"


/**************************** FOR TFT DISPLAY
 * PA4 - CS
 * PA2 - RESET
 * PA3 - DC (data command)
 * PA7 - MOSI
 * PA5 - SCK
 * PA6 - MISO
 * PA1 - LED
 * ***************************************
*
 *
 * **/

#define USART_ON  //для исследования передача по усарт


//высота букв и цифр
#define FONT_Y 8
#define FONT_X 8


// значение сброса для экрана
#define RESET_ACTIVE() GPIOA->ODR&=~GPIO_ODR_2
#define RESET_IDLE() GPIOA->ODR|=GPIO_ODR_2

//значение пина DC (команда или данные)
#define DC_COMMAND() GPIOA->ODR&=~GPIO_ODR_3
#define DC_DATA() GPIOA->ODR|=GPIO_ODR_3

// установка/сброс CS
#define SET_CS() GPIOA->ODR|=GPIO_ODR_4
#define RESET_CS() GPIOA->ODR&=~GPIO_ODR_4

//установка/сброс CS MAX6675
#define SET_CS_MAX() GPIOB->ODR|=GPIO_ODR_12
#define RESET_CS_MAX() GPIOB->ODR&=~GPIO_ODR_12

// включение подсветки
#define LED_on() GPIOA->ODR|=GPIO_ODR_1
#define LED_off() GPIOA->ODR&=~GPIO_ODR_1


// старт отправки данных по DMA и SPI
#define Start_DMA_Send_Data() DMA1_Channel3->CCR |= DMA_CCR_EN


//установка/сброс CS MAX6675
#define SET_CS_MAX() GPIOB->ODR|=GPIO_ODR_12
#define RESET_CS_MAX() GPIOB->ODR&=~GPIO_ODR_12


// режим работы (переменная mode)
#define Not_connection 0 // нет контакта
#define temp_ready 1 //температура может выводиться на экран
#define print_ready 2 // температура вывелась на экран
#define Error 3 //ошибка
#define ready_transmit 4 //передача завершена


/* For DMA - USART*/
#define BAUD 115200


#endif /* GLOBAL_H_ */

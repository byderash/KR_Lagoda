
#include "MAX6675.h"


extern uint16_t mode;

uint16_t data_MAX;
uint16_t connection=0;

void MAX6675_init(){ //инциализация термопары
	SPI2_master_init();
}

void MAX6675_get_temp(uint16_t *temp){
	if(mode==print_ready || mode == ready_transmit){ // если вывелось на экран или передача завершена
		spi2_ReceiveData_2byte(); // принимаем еще 2 байта
		if(mode==Error){ //если ошибка, выходим
			return;
		}
		uint16_t data_temp = data_MAX;
		if((data_temp & 1<<2)>>2 == 1){ //если 2й бит равен 1, то ошибка подключения
			mode= Not_connection;
			return;
		}
		data_temp = (data_temp & 0x7FF8)>>2; //избавляемся от ненужных битов
		*temp = (data_temp)>>3; // делим на 4 (так как точность 0.25 градуса)
		mode = temp_ready;
		return;
}
}

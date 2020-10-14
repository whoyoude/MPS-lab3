/*
 * Task3.c
 *
 *  Created on: Oct 9, 2020
 *      Author: You Hu
 */
#include "init.h"
#include <stdint.h>
#include<stdlib.h>


char user_m[1];
char user_s[1];
UART_HandleTypeDef uart_init1;
SPI_HandleTypeDef spi_init;

void GPIO_set_SPI();
//void read_input();
//void GPIO_set_UART();

int main(){
	Sys_Init();
	HAL_Init();
    SystemClock_Config();
	GPIO_set_SPI();

	while(1){
		user_s[0] = getchar();
		printf("\033[2J\033[;H");
		fflush(stdout);
		printf("From computer:\r\n%c",user_s[0]);
		fflush(stdout);
		printf("\033[13;H");
		fflush(stdout);
		printf("From SPI:\r\n");
		HAL_SPI_TransmitReceive(&spi_init, (uint8_t *)user_s,(uint8_t *)user_m,1, 100);
		printf("%c\r\n",user_m[0]);
		//HAL_SPI_TransmitReceive(&spi_init, (uint8_t *)user_m,(uint8_t *)user_m,1, 10);
		user_m[0] =0;
		user_s[0] =0;
		//}
	}
}


void GPIO_set_SPI(){
	__SPI2_CLK_ENABLE();

	spi_init.Instance = SPI2;
	spi_init.Init.Mode = SPI_MODE_MASTER;
	spi_init.Init.Direction = SPI_DIRECTION_2LINES;
	spi_init.Init.DataSize = SPI_DATASIZE_8BIT;
	spi_init.Init.CLKPolarity = SPI_POLARITY_HIGH;
	spi_init.Init.CLKPhase = SPI_PHASE_2EDGE;
	spi_init.Init.NSS = SPI_NSS_HARD_OUTPUT;
	spi_init.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	spi_init.Init.FirstBit = SPI_FIRSTBIT_LSB;
	spi_init.Init.TIMode = SPI_TIMODE_DISABLE;
	spi_init.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	HAL_SPI_Init(&spi_init);

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpio_init;
	gpio_init.Pin = GPIO_PIN_14 | GPIO_PIN_15;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_HIGH;
    gpio_init.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &gpio_init);
}

//void read_input(){
//
//}

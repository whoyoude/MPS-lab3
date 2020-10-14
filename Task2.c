/*
 * ask2.c
 *
 *  Created on: Oct 8, 2020
 *      Author: You Hu
 */

#include "init.h"
#include "uart.h"

UART_HandleTypeDef uart_init6;
UART_HandleTypeDef uart_init1;
char halt_info[] = "h\r\nhalt";
char user_m[1];
char user_s[1];
unsigned int stop;
unsigned int UartReady = 0;

void GPIO_set();
void readInput();
//void USART1_IRQHandler();
//void USART6_IRQHandler();
void print_message();
//void HAL_UART_RxCallBack(UART_HandleTypeDef *uart_init);

// Main Execution Loop
int main(void) {
	//Initialize the system
    Sys_Init();
	GPIO_set();
	//HAL_Init();

	readInput();
	while(1){
		if (stop){
				break;
		}
	}
}

void GPIO_set(){
//	HAL_Init();
   // SystemClock_Config();

    HAL_NVIC_SetPriority(USART1_IRQn,0,1);
    HAL_NVIC_SetPriority(USART6_IRQn,0,0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_EnableIRQ(USART6_IRQn);

    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

	uart_init6.Instance = USART6;
	uart_init6.Init.BaudRate = 38400;
	uart_init6.Init.WordLength = UART_WORDLENGTH_8B;
	uart_init6.Init.StopBits = UART_STOPBITS_1;
	uart_init6.Init.Parity     = UART_PARITY_NONE;
	uart_init6.Init.Mode       = UART_MODE_TX_RX;
	uart_init6.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	HAL_UART_Init(&uart_init6);
	//HAL_UART_MspInit(&uart_init6);
	//__HAL_UART_ENABLE_IT(&uart_init6, UART_IT_RXNE);

	uart_init1.Instance = USART1;
	uart_init1.Init.BaudRate = 115200;
	uart_init1.Init.WordLength = UART_WORDLENGTH_8B;
	uart_init1.Init.StopBits = UART_STOPBITS_1;
	uart_init1.Init.Parity     = UART_PARITY_NONE;
	uart_init1.Init.Mode       = UART_MODE_TX_RX;
	uart_init1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	HAL_UART_Init(&uart_init1);
	//HAL_UART_MspInit(&uart_init1);
	//__HAL_UART_ENABLE_IT(&uart_init1, UART_IT_RXNE);

}

void readInput(){
	HAL_UART_Receive_IT(&uart_init1, (uint8_t *)user_m, 1);
	HAL_UART_Receive_IT(&uart_init6, (uint8_t *)user_s, 1);
}

void USART1_IRQHandler(){
	HAL_UART_IRQHandler(&uart_init1);
}

void USART6_IRQHandler(){
	HAL_UART_IRQHandler(&uart_init6);
}

void print_message(){
	if(user_s[0] != 0){
		HAL_UART_Transmit_IT(&uart_init1, (uint8_t*) user_s,1);
		HAL_UART_Transmit_IT(&uart_init6, (uint8_t *)user_s, 1);
		}
		if(user_m[0] != 0){
		HAL_UART_Transmit_IT(&uart_init1, (uint8_t*) user_m,1);
		HAL_UART_Transmit_IT(&uart_init6, (uint8_t *)user_m, 1);
		}
		//HAL_Delay(100);
		if(user_s[0] == 27 | user_m[0] == 27 ){
		    HAL_UART_Transmit_IT(&uart_init1, (uint8_t*) halt_info, strlen(halt_info));
		    HAL_UART_Transmit_IT(&uart_init6,  (uint8_t *)user_m, 1);
		    return 0;
		}
		user_s[0] = 0;
		user_m[0] = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart_init){
	if(uart_init->Instance == USART6){ //Nucleo
			HAL_UART_Receive_IT(&uart_init6, (uint8_t *)user_s, 1);
		}
		if(uart_init->Instance == USART1){ //STM32
			HAL_UART_Receive_IT(&uart_init1, (uint8_t *)user_m, 1);
		}
	//printf("hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\r\n");
	UartReady = 1;
	//printf("userm %c; users %c", user_m[0],user_s[0]);
	if(user_s[0] != 0){
		HAL_UART_Transmit(&uart_init1, (uint8_t*) user_s,1,10);
		HAL_UART_Transmit(&uart_init6, (uint8_t*)user_s, 1,10);
	}
	if(user_m[0] != 0){
		HAL_UART_Transmit(&uart_init1, (uint8_t*) user_m,1,10);
		HAL_UART_Transmit(&uart_init6, (uint8_t*)user_m, 1,10);
	}
		//HAL_Delay(100);
	if(user_s[0] == 27 | user_m[0] == 27 ){
		HAL_UART_Transmit(&uart_init1, (uint8_t*) halt_info, strlen(halt_info),10);
		HAL_UART_Transmit(&uart_init6,  (uint8_t *)user_m, 1,10);
		stop =1;//If stop ==1 program halt
		   // return 0;
    }
	user_s[0] = 0;
	user_m[0] = 0;
//	if(uart_init->Instance == USART6){
//		HAL_UART_Receive_IT(&uart_init6, (uint8_t *)user_s, 1);
//	}
//	if(uart_init->Instance == USART1){
//		HAL_UART_Receive_IT(&uart_init1, (uint8_t *)user_m, 1);
//	}
}

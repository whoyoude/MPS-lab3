#include "init.h"
#include "uart.h"

UART_HandleTypeDef uart_init6;
UART_HandleTypeDef uart_init1;
char halt_info[] = "h\r\nhalt";
char user_m[1];
char user_s[1];

void GPIO_set();
void readInput();


// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();
	GPIO_set();

	while(1){
		readInput();
		if(user_s[0] != 0){//Determine if signal is from Nucleo board
		HAL_UART_Transmit(&uart_init1, (uint8_t*) user_s,1, 10);
		HAL_UART_Transmit(&uart_init6, (uint8_t *)user_s, 1, 10);
		}
		if(user_m[0] != 0){//Determine if signal is from DISCO board
		HAL_UART_Transmit(&uart_init1, (uint8_t*) user_m,1, 10);
		HAL_UART_Transmit(&uart_init6, (uint8_t *)user_m, 1, 10);
		}
		if(user_s[0] == 27 | user_m[0] == 27 ){//Determine if signal is ESC
		    HAL_UART_Transmit(&uart_init1, (uint8_t*) halt_info, strlen(halt_info), 10);
		    HAL_UART_Transmit(&uart_init6,  (uint8_t *)user_m, 1, 10);
		    return 0;
		}
		//Clear both arrays
		user_s[0] = 0;
		user_m[0] = 0;
	}
}

void GPIO_set(){
	HAL_Init();
    SystemClock_Config();
	//Initiate USART6
	uart_init6.Instance = USART6;
	uart_init6.Init.BaudRate = 38400;
	uart_init6.Init.WordLength = UART_WORDLENGTH_8B;
	uart_init6.Init.StopBits = UART_STOPBITS_1;
	uart_init6.Init.Parity     = UART_PARITY_NONE;
	uart_init6.Init.Mode       = UART_MODE_TX_RX;
	uart_init6.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	HAL_UART_Init(&uart_init6);
	HAL_UART_MspInit(&uart_init6);
        //Initiate USART1
	uart_init1.Instance = USART1;
	uart_init1.Init.BaudRate = 115200;
	uart_init1.Init.WordLength = UART_WORDLENGTH_8B;
	uart_init1.Init.StopBits = UART_STOPBITS_1;
	uart_init1.Init.Parity     = UART_PARITY_NONE;
	uart_init1.Init.Mode       = UART_MODE_TX_RX;
	uart_init1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	HAL_UART_Init(&uart_init1);
	HAL_UART_MspInit(&uart_init1);
}

void readInput(){//Receive data from both USART1 and USART6
	HAL_UART_Receive(&uart_init1, (uint8_t *)user_m, 1, 10);
	HAL_UART_Receive(&uart_init6, (uint8_t *)user_s, 1, 10);
}

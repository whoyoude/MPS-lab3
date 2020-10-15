//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * For convenience, configure the SPI handler here
 */
static SPI_HandleTypeDef spi_config;
int item=2;
uint8_t storage[10]={1,0,0,0x01,0,0,0,0,0,0x09};
uint8_t reg;
int adc_flag=0;
int ready=0;
uint16_t temp=0;
uint8_t input=0;
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c

ADC_HandleTypeDef hADC1;

void initTempSensor(){
	__HAL_RCC_ADC1_CLK_ENABLE();

	hADC1.Instance = ADC1;
	hADC1.Init.Resolution = ADC_RESOLUTION_12B;
	hADC1.Init.NbrOfConversion = 1;
	hADC1.Init.ContinuousConvMode = DISABLE;
	hADC1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hADC1.Init.ExternalTrigConv = ADC_SOFTWARE_START;

	HAL_ADC_Init(&hADC1);

	//ADC->CCR |= ADC_CCR_TSVREFE;
	ADC_ChannelConfTypeDef adcchan = {  .Channel = ADC_CHANNEL_TEMPSENSOR,
										.Rank = ADC_REGULAR_RANK_1,
										.SamplingTime = ADC_SAMPLETIME_480CYCLES};
	HAL_ADC_ConfigChannel(&hADC1,&adcchan);
}

uint16_t getTemperature(){
	HAL_ADC_Start(&hADC1);
	HAL_StatusTypeDef res =  HAL_ADC_PollForConversion(&hADC1,100);
	if(res == HAL_OK){
		return HAL_ADC_GetValue(&hADC1);
	}
	return 0;
}




void configureSPI()
{
	spi_config.Instance = SPI2; // Please use SPI2!
	spi_config.Init.Mode = SPI_MODE_SLAVE; // Set master mode
	spi_config.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	spi_config.Init.NSS = SPI_NSS_SOFT;
	spi_config.Init.Direction = SPI_DIRECTION_2LINES;
	spi_config.Init.DataSize = SPI_DATASIZE_8BIT;
	spi_config.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	spi_config.Init.CLKPhase = SPI_PHASE_2EDGE;
	spi_config.Init.CLKPolarity = SPI_POLARITY_LOW;
	spi_config.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi_config.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi_config.Init.CRCPolynomial = 7;
	spi_config.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	spi_config.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

/*
 * ... You get the idea.
 */
	HAL_SPI_Init(&spi_config);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	//
	// Note: HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
	//
	// HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properties of
	// the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
	// the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
 // Do NOT change the name of an MspInit function; it needs to override a
 // __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// SPI GPIO initialization structure here
	GPIO_InitTypeDef  GPIO_InitStruct;

	if (hspi->Instance == SPI2)
	{
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// Enable GPIO Clocks
	    __HAL_RCC_SPI2_CLK_ENABLE();

	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();
		// Initialize SPI2_SCK PA12 D13 SPI2_NSS PA11 D10
	    GPIO_InitStruct.Pin = GPIO_PIN_12;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    __HAL_RCC_GPIOJ_CLK_ENABLE();

		GPIO_InitStruct.Pin       = GPIO_PIN_4;
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

		GPIO_InitStruct.Pin       = GPIO_PIN_11;
		GPIO_InitStruct.Mode      = GPIO_MODE_IT_RISING_FALLING;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
		__HAL_RCC_SPI2_CLK_ENABLE();
		//Set priority and enable the IRQ

		HAL_NVIC_SetPriority(SPI2_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);

		__HAL_RCC_SYSCFG_CLK_ENABLE();
		HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}



void EXTI15_10_IRQHandler(){
	EXTI->PR &= EXTI_PR_PR11;
	if(HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_4)==GPIO_PIN_RESET){
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_SPI_Receive_IT(&spi_config, &reg, 1);
	}else{
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Abort(&spi_config);
	}
	//HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}


/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)){
	case GPIO_PIN_SET: state=1;break;
	case GPIO_PIN_RESET: state=3;break;
	}
	printf("\033[1;11r\033[2;1H\033[2K\n");
	printf("Pushed D4, global varaible set\r\n");

}*/

void SPI2_IRQHandler(){
	uint8_t data;
	ready=1;
	HAL_SPI_IRQHandler(&spi_config);
	if(ready){/*
		printf("\033[13;1H\033[2K");
		fflush(stdout);
		printf("[%x]",reg);
		fflush(stdout);*/
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
		switch(reg){
		case 0:
			HAL_SPI_Transmit(&spi_config, &storage[0], 1,100);
			break;
		case 1:;
			HAL_SPI_Transmit(&spi_config, &storage[1], 1,100);
			break;
		case 2:
			HAL_SPI_Receive(&spi_config, &storage[2], 1,100);
			if ((storage[2]|0x02) == 0x02) {
				adc_flag=1;
				storage[3]|=0x05;
				storage[3]&=0xf5;
			}
			break;
		case 3:;
			HAL_SPI_Transmit(&spi_config, &storage[3], 1,100);
			storage[3]&=0xef;
			break;
		case 5:
			//storage[5]=0x20;
			HAL_SPI_Transmit(&spi_config, &storage[5], 1,100);
			break;
		case 6:
			//storage[6]=0x10;
			HAL_SPI_Transmit(&spi_config, &storage[6], 1,100);
			storage[3]&=0xf7;
			break;
		case 7:
			HAL_SPI_TransmitReceive(&spi_config, &input,&data, 1, 100);
			if(data){
				printf("\033[13;24r\033[24;1H\n");
				storage[7]=data;
				printf("%c from master",storage[7]);
				fflush(stdout);
			}
			storage[3]&=0xDF;
			input=0;
			break;
		case 9:
			HAL_SPI_Transmit(&spi_config, &storage[9], 1,100);
			break;
		}
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
		ready=0;
	}
}

/*void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){

}*/


int main(void)
{
	Sys_Init();

	printf("\033[0m\033[2J\033[12;1H\033[s");
	fflush(stdout);
	// For convenience

	//initTempSensor();
	HAL_Delay(20);
	configureSPI();
	initTempSensor();

	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
	//HAL_SPI_Receive_IT(&spi_config, reg, 1);
	while(1){
		HAL_UART_Receive(&USB_UART, &input, 1, 100);
		if(input){
			printf("\033[1;12r\033[12;1H");
			fflush(stdout);
			printf("\n%c from usb",input);
			fflush(stdout);
			storage[3]|=0x20;
		}
		if(adc_flag){
			uint16_t full_temp=956;
			full_temp = getTemperature();


			storage[5]=full_temp;
			storage[6]=full_temp>>8;
			adc_flag=0;
			storage[3]|=0x19;
			storage[3]&=0xf9;
		}


	}


// See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}

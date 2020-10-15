//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"
// If needed:
#include <stdio.h>
#include <stdlib.h>

/*
 * For convenience, configure the SPI handler here
 */
static SPI_HandleTypeDef spi_config;
int item=2;
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void configureSPI()
{
	spi_config.Instance = SPI2; // Please use SPI2!
	spi_config.Init.Mode = SPI_MODE_MASTER; // Set master mode
	spi_config.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	spi_config.Init.NSS = SPI_NSS_HARD_OUTPUT;
	spi_config.Init.Direction = SPI_DIRECTION_2LINES;
	spi_config.Init.DataSize = SPI_DATASIZE_8BIT;
	spi_config.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	spi_config.Init.CLKPhase = SPI_PHASE_2EDGE;
	spi_config.Init.CLKPolarity = SPI_POLARITY_LOW;
	spi_config.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi_config.Init.FirstBit = SPI_FIRSTBIT_MSB;

/*
 * ... You get the idea.
 */

	HAL_SPI_Init(&spi_config);
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
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
		__GPIOJ_CLK_ENABLE();

		// Initialize SPI2_SCK PA12 D13 SPI2_NSS PA11 D10
		GPIO_InitStruct.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //SPI2_SCK Config

		// Initialize SPI2_MISO PB14 D12 SPI2_MOSI PB15 D11
		GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); //SPI2_MISO Config

		GPIO_InitStruct.Pin       = GPIO_PIN_4;
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
		__SPI2_CLK_ENABLE();
	}
}



void read_version(){
	printf("\033[0m\033[2J\033[1;1H");
	fflush(stdout);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(1);
	uint8_t max[1]={0};

	HAL_SPI_Transmit(&spi_config, max, 1, 100);
	HAL_Delay(1);
	HAL_SPI_Receive(&spi_config, max, 1, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);

	uint8_t min[1]={1};

	HAL_SPI_Transmit(&spi_config, min, 1, 100);
	HAL_Delay(1);
	HAL_SPI_Receive(&spi_config, min, 1, 100);
	printf("\033[1;1Hversion: %u.%u",max[0],min[0]);
	fflush(stdout);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(2000);

}

void change_id(){
	printf("\033[0m\033[1;1H");
	fflush(stdout);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(1);
	uint8_t data[1]={9};

	HAL_SPI_Transmit(&spi_config, data, 1, 100);
	HAL_Delay(1);
	HAL_SPI_Receive(&spi_config, data, 1, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
	printf("Current ID: %u  ", data[0]);
	fflush(stdout);

	uint8_t i;
	printf("New ID: ");
	fflush(stdout);
	scanf("%u", &i);
	printf("%u", i);
	fflush(stdout);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(1);
	data[0]=2;

	HAL_SPI_Transmit(&spi_config, data, 1, 100);
	HAL_Delay(1);
	data[0]=0x80;
	HAL_SPI_Transmit(&spi_config, data, 1, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);

	data[0]=9;

	HAL_SPI_Transmit(&spi_config, data, 1, 100);
	HAL_Delay(1);
	data[0]=i;
	HAL_SPI_Transmit(&spi_config, data, 1, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
}

void loop(){

	uint8_t i=0;
	uint8_t data=7;
	HAL_UART_Receive(&USB_UART, &i, 1, 100);
	if(i){
		printf("\033[1;12r\033[0m\033[12;1H");
		fflush(stdout);
		printf("\n\r%c",i);
		if(i==0x1b){
			printf("\033[1;20H");
			fflush(stdout);
			printf("1.read temp 2.send char 3. modify id 4.clear terminal");
			fflush(stdout);
			char c = getchar();
			printf("\033[0m\033[2K\033[1;1H");
			fflush(stdout);
			switch (c){
			case '1': item=1; break;
			case '2': item=2; break;
			case '3': item=3; break;
			case '4': item=4; break;
			}
		}
		if(item==1){
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
			HAL_Delay(1);
			data=2;
			HAL_SPI_Transmit(&spi_config, &data, 1, 100);
			HAL_Delay(1);
			data=0x02;
			HAL_SPI_Transmit(&spi_config, &data, 1, 100);
			HAL_Delay(1);
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
			HAL_Delay(1);
			while(1){
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
				HAL_Delay(1);
				uint8_t data=3;
				HAL_SPI_Transmit(&spi_config, &data, 1, 100);
				HAL_Delay(1);
				HAL_SPI_Receive(&spi_config, &data, 1, 100);

				HAL_Delay(1);
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
				HAL_Delay(1);
				if(data&0x08==0x08) break;
			}
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
			HAL_Delay(1);
			data=5;
			HAL_SPI_Transmit(&spi_config, &data, 1, 100);
			HAL_Delay(1);
			HAL_SPI_Receive(&spi_config, &data, 1, 100);
			HAL_Delay(1);
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
			HAL_Delay(1);
			uint16_t temp=0x0000 | data;

			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
			HAL_Delay(1);
			data=6;
			HAL_SPI_Transmit(&spi_config, &data, 1, 100);
			HAL_Delay(1);
			HAL_SPI_Receive(&spi_config, &data, 1, 100);
			HAL_Delay(1);
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
			HAL_Delay(1);
			temp=temp | (data<<8);
			printf("\033[5;40H");
			fflush(stdout);
			printf("Temperature is: %d degree Celcius    ", temp*0.322-279);
			fflush(stdout);
			printf("\033[6;40H");
			fflush(stdout);
			printf("Raw temperature is: %d    ", temp);
			fflush(stdout);
			item=2;

		}

		if(item==3){
			change_id();
			item=2;
			printf("\033[0m\033[2K\033[1;1H");
			fflush(stdout);
		}
		if(item==4){
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
			HAL_Delay(1);
			uint8_t data[1]={2};
			HAL_SPI_Transmit(&spi_config, data, 1, 100);
			HAL_Delay(1);
			data[0]=0x04;
			HAL_SPI_Transmit(&spi_config, data, 1, 100);
			HAL_Delay(1);
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
			HAL_Delay(1);
			item=2;
		}
		return;
	}

	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_Transmit(&spi_config, &data, 1, 100);
	HAL_Delay(1);

	HAL_SPI_TransmitReceive(&spi_config, &i,&data, 1, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);

    printf("\033[13;24r");//set scroll section
    fflush(stdout);
	printf("\033[0m\033[24;1H");
	fflush(stdout);


	if(data){
		printf("\n\rFROM SLAVE: %c",data);
		fflush(stdout);
	}


}


int main(void)
{
	Sys_Init();
	printf("\033[0m\033[2J\033[1;1H\033[12;1H");
	fflush(stdout);
	// For convenience
	configureSPI();
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	read_version();
	read_version();
	while(1){
		loop();
	}


// See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}

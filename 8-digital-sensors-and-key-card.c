/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"string.h"
#include"stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define BuffSize 64

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint16_t GPIODATA[BuffSize]={0};
uint16_t TEMPDATA[BuffSize]={0};
uint8_t  READDATA[BuffSize]={0};
uint8_t  SENDATA[BuffSize]={0};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
 char byte[5];
 int a=0;
 char buffer[100];
 int i=0;
 signed char swbyte[16]={0};
 signed char tempbyte[16]={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Temp1_byte1,Temp1_byte2,Temp2_byte1,Temp2_byte2,Temp3_byte1,Temp3_byte2,Temp4_byte1,Temp4_byte2;
uint8_t Temp5_byte1,Temp5_byte2,Temp6_byte1,Temp6_byte2,Temp7_byte1,Temp7_byte2,Temp8_byte1,Temp8_byte2;
int TEMP1,TEMP2,TEMP3,TEMP4,TEMP5,TEMP6,TEMP7,TEMP8;

float Temperature1=0,Temperature2=0,Temperature3=0,Temperature4=0;
float Temperature5=0,Temperature6=0,Temperature7=0,Temperature8=0;
float Temperature9=-20;
float Humidity=0;
uint8_t Presence1=0,Presence2=0,Presence3=0,Presence4=0;
uint8_t Presence5=0,Presence6=0,Presence7=0,Presence8=0;

uint8_t Rx_data;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart)
{
	HAL_UART_Receive_IT(&huart1, &Rx_data, 1);
}

	void delay_us (uint16_t us)
	{
		__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
		while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
	}

	//uint8_t Rh_byte1,Rh_byte2,Temp_byte1,Temp_byte2;
	//int SUM,RH,TEMP;





	void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
	{
		GPIO_InitTypeDef GPIO_InitStruct ={0};
		GPIO_InitStruct.Pin=GPIO_Pin;
		GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}
	void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
		{
			GPIO_InitTypeDef GPIO_InitStruct ={0};
			GPIO_InitStruct.Pin=GPIO_Pin;
			GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull=GPIO_PULLUP;
			HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
		}

	/************************** DS18B20 Functions*************************/
#define DS18B20_PORT GPIOA
#define DS18B20_PIN_0 GPIO_PIN_0//Temp1 pin
#define DS18B20_PIN_1 GPIO_PIN_1//Temp2 pin
#define DS18B20_PIN_2 GPIO_PIN_2//Temp3 pin
#define DS18B20_PIN_3 GPIO_PIN_3//Temp4 pin
#define DS18B20_PIN_4 GPIO_PIN_4//Temp5 pin
#define DS18B20_PIN_5 GPIO_PIN_5//Temp6 pin
#define DS18B20_PIN_6 GPIO_PIN_6//Temp7 pin
#define DS18B20_PIN_7 GPIO_PIN_7//Temp8 pin
/*
	void DS18B20_Port_Selection(void)
	{
		int Port=1;
		if(Port==1)
		{

		}

		else if(Port==0)
			{

	#define DS18B20_PORT GPIOA
	#define DS18B20_PIN GPIO_PIN_2
			}
	}
*/
	/**********************Temp_1*****************************/
		uint8_t DS18B20_Start_0 (void)
			{
				uint8_t Response = 0;
				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_0);   // set the pin as output
				HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_0, 0);  // pull the pin low
				delay_us (480);   // delay according to datasheet

				Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_0);    // set the pin as input
				delay_us (80);    // delay according to datasheet

				if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_0))) Response = 1;    // if the pin is low i.e the presence pulse is detected
				else Response = -1;

				delay_us (400); // 480 us delay totally.

				return Response;
			}

		void DS18B20_Write_0 (uint8_t data)
			{

				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_0);  // set as output

				for (int i=0; i<8; i++)
				{

					if ((data & (1<<i))!=0)  // if the bit is high
					{
						// write 1

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_0);  // set as output
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_0, 0);  // pull the pin LOW
						delay_us (1);  // wait for 1 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_0);  // set as input
						delay_us (50);  // wait for 60 us
					}

					else  // if the bit is low
					{
						// write 0

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_0);
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_0, 0);  // pull the pin LOW
						delay_us(50);  // wait for 60 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_0);
					}
				}
			}

		uint8_t DS18B20_Read_0 (void)
			{

				uint8_t value=0;
				Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_0);

				for (int i=0;i<8;i++)
				{
					Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_0);   // set as output

					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_0, 0);  // pull the data pin LOW
					delay_us (2);  // wait for 2 us

					Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_0);  // set as input
					if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_0))  // if the pin is HIGH
					{
						value |= 1<<i;  // read = 1
					}
					delay_us (60);  // wait for 60 us
				}
				return value;
			}

	   /******************************Temp_2**********************/
		uint8_t DS18B20_Start_1 (void)
			{
				uint8_t Response = 0;
				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_1);   // set the pin as output
				HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_1, 0);  // pull the pin low
				delay_us (480);   // delay according to datasheet

				Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_1);    // set the pin as input
				delay_us (80);    // delay according to datasheet

				if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_1))) Response = 1;    // if the pin is low i.e the presence pulse is detected
				else Response = -1;

				delay_us (400); // 480 us delay totally.

				return Response;
			}

		void DS18B20_Write_1 (uint8_t data)
			{

				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_1);  // set as output

				for (int i=0; i<8; i++)
				{

					if ((data & (1<<i))!=0)  // if the bit is high
					{
						// write 1

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_1);  // set as output
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_1, 0);  // pull the pin LOW
						delay_us (1);  // wait for 1 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_1);  // set as input
						delay_us (50);  // wait for 60 us
					}

					else  // if the bit is low
					{
						// write 0

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_1);
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_1, 0);  // pull the pin LOW
						delay_us(50);  // wait for 60 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_1);
					}
				}
			}

		uint8_t DS18B20_Read_1 (void)
			{

				uint8_t value=0;
				Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_1);

				for (int i=0;i<8;i++)
				{
					Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_1);   // set as output

					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_1, 0);  // pull the data pin LOW
					delay_us (2);  // wait for 2 us

					Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_1);  // set as input
					if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_1))  // if the pin is HIGH
					{
						value |= 1<<i;  // read = 1
					}
					delay_us (60);  // wait for 60 us
				}
				return value;
			}
	  /**************************Temp_3************************/
		uint8_t DS18B20_Start_2 (void)
			{
				uint8_t Response = 0;
				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_2);   // set the pin as output
				HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_2, 0);  // pull the pin low
				delay_us (480);   // delay according to datasheet

				Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_2);    // set the pin as input
				delay_us (80);    // delay according to datasheet

				if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_2))) Response = 1;    // if the pin is low i.e the presence pulse is detected
				else Response = -1;

				delay_us (400); // 480 us delay totally.

				return Response;
			}

		void DS18B20_Write_2 (uint8_t data)
			{

				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_2);  // set as output

				for (int i=0; i<8; i++)
				{

					if ((data & (1<<i))!=0)  // if the bit is high
					{
						// write 1

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_2);  // set as output
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_2, 0);  // pull the pin LOW
						delay_us (1);  // wait for 1 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_2);  // set as input
						delay_us (50);  // wait for 60 us
					}

					else  // if the bit is low
					{
						// write 0

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_2);
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_2, 0);  // pull the pin LOW
						delay_us(50);  // wait for 60 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_2);
					}
				}
			}

		uint8_t DS18B20_Read_2 (void)
			{

				uint8_t value=0;
				Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_2);

				for (int i=0;i<8;i++)
				{
					Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_2);   // set as output

					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_2, 0);  // pull the data pin LOW
					delay_us (2);  // wait for 2 us

					Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_2);  // set as input
					if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_2))  // if the pin is HIGH
					{
						value |= 1<<i;  // read = 1
					}
					delay_us (60);  // wait for 60 us
				}
				return value;
			}
		/***************************Temp_4********************/
		uint8_t DS18B20_Start_3 (void)
				{
					uint8_t Response = 0;
					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_3);   // set the pin as output
					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_3, 0);  // pull the pin low
					delay_us (480);   // delay according to datasheet

					Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_3);    // set the pin as input
					delay_us (80);    // delay according to datasheet

					if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_3))) Response = 1;    // if the pin is low i.e the presence pulse is detected
					else Response = -1;

					delay_us (400); // 480 us delay totally.

					return Response;
				}

		void DS18B20_Write_3 (uint8_t data)
				{

				Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_3);  // set as output

				for (int i=0; i<8; i++)
					{

					if ((data & (1<<i))!=0)  // if the bit is high
						{
							// write 1

							Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_3);  // set as output
							HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_3, 0);  // pull the pin LOW
							delay_us (1);  // wait for 1 us

							Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_3);  // set as input
							delay_us (50);  // wait for 60 us
						}

						else  // if the bit is low
						{
							// write 0

							Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_3);
							HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_3, 0);  // pull the pin LOW
							delay_us(50);  // wait for 60 us

							Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_3);
						}
					}
				}

		uint8_t DS18B20_Read_3 (void)
				{

					uint8_t value=0;
					Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_3);

					for (int i=0;i<8;i++)
					{
						Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_3);   // set as output

						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_3, 0);  // pull the data pin LOW
						delay_us (2);  // wait for 2 us

						Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_3);  // set as input
						if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_3))  // if the pin is HIGH
						{
							value |= 1<<i;  // read = 1
						}
						delay_us (60);  // wait for 60 us
					}
					return value;
				}

	   /********************Temp_5**************************/

		uint8_t DS18B20_Start_4 (void)
				{
					uint8_t Response = 0;
					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_4);   // set the pin as output
					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_4, 0);  // pull the pin low
					delay_us (480);   // delay according to datasheet

					Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_4);    // set the pin as input
					delay_us (80);    // delay according to datasheet

					if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_4))) Response = 1;    // if the pin is low i.e the presence pulse is detected
					else Response = -1;
					delay_us (400); // 480 us delay totally.

					return Response;
				}

		void DS18B20_Write_4 (uint8_t data)
					{

					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_4);  // set as output

					for (int i=0; i<8; i++)
						{
							if ((data & (1<<i))!=0)  // if the bit is high
							{
									// write 1

						Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_4);  // set as output
						HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_4, 0);  // pull the pin LOW
						delay_us (1);  // wait for 1 us

						Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_4);  // set as input
						delay_us (50);  // wait for 60 us
						}
							else  // if the bit is low
							{
									// write 0

							Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_4);
							HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_4, 0);  // pull the pin LOW
							delay_us(50);  // wait for 60 us

							Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_4);
								}
							}
						}

		uint8_t DS18B20_Read_4 (void)
					{

						uint8_t value=0;
						Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_4);
						for (int i=0;i<8;i++)
						{
							Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_4);   // set as output

							HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_4, 0);  // pull the data pin LOW
							delay_us (2);  // wait for 2 us

							Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_4);  // set as input
						if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_4))  // if the pin is HIGH
						  {
						   	value |= 1<<i;  // read = 1
					    	}
							delay_us (60);  // wait for 60 us
						}
							return value;
						}
		/*********************Temp_6**********************/
		uint8_t DS18B20_Start_5 (void)
										{
											uint8_t Response = 0;
											Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_5);   // set the pin as output
											HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_5, 0);  // pull the pin low
											delay_us (480);   // delay according to datasheet

											Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_5);    // set the pin as input
											delay_us (80);    // delay according to datasheet

											if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_5))) Response = 1;    // if the pin is low i.e the presence pulse is detected
											else Response = -1;

											delay_us (400); // 480 us delay totally.

											return Response;
										}

		void DS18B20_Write_5 (uint8_t data)
						{

										Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_5);  // set as output
										for (int i=0; i<8; i++)
										{

											if ((data & (1<<i))!=0)  // if the bit is high
											{
													// write 1

												Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_5);  // set as output
												HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_5, 0);  // pull the pin LOW
												delay_us (1);  // wait for 1 us

												Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_5);  // set as input
												delay_us (50);  // wait for 60 us
											}

											else  // if the bit is low
											{
													// write 0
												Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_5);
												HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_5, 0);  // pull the pin LOW
												delay_us(50);  // wait for 60 us

												Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_5);
											}
										}
									}

	   uint8_t DS18B20_Read_5 (void)
									{

									uint8_t value=0;
									Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_5);

								for (int i=0;i<8;i++)
									{
										Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_5);   // set as output

										HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_5, 0);  // pull the data pin LOW
										delay_us (2);  // wait for 2 u
										Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_5);  // set as input
										if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_5))  // if the pin is HIGH
										{
											value |= 1<<i;  // read = 1
										}
										delay_us (60);  // wait for 60 us
									}
							return value;
								}
	      /*******************Temp_7**********************/
	   uint8_t DS18B20_Start_6 (void)
	   		{
	   			uint8_t Response = 0;
	   			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_6);   // set the pin as output
	   			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_6, 0);  // pull the pin low
	   			delay_us (480);   // delay according to datasheet

	   			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_6);    // set the pin as input
	   			delay_us (80);    // delay according to datasheet

	   			if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_6))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	   			else Response = -1;

	   			delay_us (400); // 480 us delay totally.

	   			return Response;
	   		}

	   	void DS18B20_Write_6 (uint8_t data)
	   		{

	   			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_6);  // set as output

	   			for (int i=0; i<8; i++)
	   			{

	   				if ((data & (1<<i))!=0)  // if the bit is high
	   				{
	   					// write 1

	   					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_6);  // set as output
	   					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_6, 0);  // pull the pin LOW
	   					delay_us (1);  // wait for 1 us

	   					Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_6);  // set as input
	   					delay_us (50);  // wait for 60 us
	   				}

	   				else  // if the bit is low
	   				{
	   					// write 0

	   					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_6);
	   					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_6, 0);  // pull the pin LOW
	   					delay_us(50);  // wait for 60 us

	   					Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_6);
	   				}
	   			}
	   		}

	   	uint8_t DS18B20_Read_6 (void)
	   		{

	   			uint8_t value=0;
	   			Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_6);

	   			for (int i=0;i<8;i++)
	   			{
	   				Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_6);   // set as output

	   				HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_6, 0);  // pull the data pin LOW
	   				delay_us (2);  // wait for 2 us

	   				Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_6);  // set as input
	   				if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_6))  // if the pin is HIGH
	   				{
	   					value |= 1<<i;  // read = 1
	   				}
	   				delay_us (60);  // wait for 60 us
	   			}
	   			return value;
	   		}
	   	/*********************Temp_8***********************/
	   	uint8_t DS18B20_Start_7 (void)
	   	   		{
	   	   			uint8_t Response = 0;
	   	   			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_7);   // set the pin as output
	   	   			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_7, 0);  // pull the pin low
	   	   			delay_us (480);   // delay according to datasheet

	   	   			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_7);    // set the pin as input
	   	   			delay_us (80);    // delay according to datasheet

	   	   			if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN_7))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	   	   			else Response = -1;

	   	   			delay_us (400); // 480 us delay totally.

	   	   			return Response;
	   	   		}

	   	void DS18B20_Write_7 (uint8_t data)
	   	   		{

	   	   			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_7);  // set as output

	   	   			for (int i=0; i<8; i++)
	   	   			{

	   	   				if ((data & (1<<i))!=0)  // if the bit is high
	   	   				{
	   	   					// write 1

	   	   					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_7);  // set as output
	   	   					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_7, 0);  // pull the pin LOW
	   	   					delay_us (1);  // wait for 1 us

	   	   					Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_7);  // set as input
	   	   					delay_us (50);  // wait for 60 us
	   	   				}

	   	   				else  // if the bit is low
	   	   				{
	   	   					// write 0

	   	   					Set_Pin_Output(DS18B20_PORT, DS18B20_PIN_7);
	   	   					HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_7, 0);  // pull the pin LOW
	   	   					delay_us(50);  // wait for 60 us

	   	   					Set_Pin_Input(DS18B20_PORT, DS18B20_PIN_7);
	   	   				}
	   	   			}
	   	   		}

	   	uint8_t DS18B20_Read_7 (void)

	   	   		{

	   	   			uint8_t value=0;
	   	   			Set_Pin_Input (DS18B20_PORT, DS18B20_PIN_7);

	   	   			for (int i=0;i<8;i++)
	   	   			{
	   	   				Set_Pin_Output (DS18B20_PORT, DS18B20_PIN_7);   // set as output

	   	   				HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN_7, 0);  // pull the data pin LOW
	   	   				delay_us (2);  // wait for 2 us

	   	   				Set_Pin_Input( DS18B20_PORT,DS18B20_PIN_7);  // set as input
	   	   				if (HAL_GPIO_ReadPin (GPIOA, DS18B20_PIN_7))  // if the pin is HIGH
	   	   				{
	   	   					value |= 1<<i;  // read = 1
	   	   				}
	   	   				delay_us (60);  // wait for 60 us
	   	   			}
	   	   			return value;
	   	   		}
		void Temp1_Funciton(void)
	   	{
	     Presence1 = DS18B20_Start_0 ();
	   	 HAL_Delay (1);
	   	 DS18B20_Write_0 (0xCC);  // skip ROM
	     DS18B20_Write_0 (0x44);  // convert t
	     HAL_Delay (50);

	     Presence1 = DS18B20_Start_0 ();
	     HAL_Delay(1);
	     DS18B20_Write_0 (0xCC);  // skip ROM
	     DS18B20_Write_0 (0xBE);  // Read Scratch-pad

	     Temp1_byte1 = DS18B20_Read_0();
	     Temp1_byte2 = DS18B20_Read_0();
	   	 TEMP1=(Temp1_byte2<<8)|Temp1_byte1;
	     Temperature1=(float)TEMP1/16;
	   	}
		void Temp2_Funciton(void)
	   	{
		Presence2 = DS18B20_Start_1 ();
	    HAL_Delay (1);
	    DS18B20_Write_1 (0xCC);  // skip ROM
	    DS18B20_Write_1 (0x44);  // convert t
	    HAL_Delay (50);

	    Presence2 = DS18B20_Start_1 ();
	    HAL_Delay(1);
	    DS18B20_Write_1 (0xCC);  // skip ROM
	    DS18B20_Write_1 (0xBE);  // Read Scratch-pad

	    Temp2_byte1 = DS18B20_Read_1();
	    Temp2_byte2 = DS18B20_Read_1();
	    TEMP2=(Temp2_byte2<<8)|Temp2_byte1;
	    Temperature2=(float)TEMP2/16;
	   	}
		void Temp3_Funciton(void)
	   	{
	    Presence3 = DS18B20_Start_2 ();
	       HAL_Delay (1);
		   DS18B20_Write_2 (0xCC);  // skip ROM
		   DS18B20_Write_2 (0x44);  // convert t
		   HAL_Delay (50);

		   Presence3 = DS18B20_Start_2 ();
		   HAL_Delay(1);
		   DS18B20_Write_2 (0xCC);  // skip ROM
		   DS18B20_Write_2 (0xBE);  // Read Scratch-pad

			Temp3_byte1 = DS18B20_Read_2();
			Temp3_byte2 = DS18B20_Read_2();
			TEMP3=(Temp3_byte2<<8)|Temp3_byte1;
			Temperature3=(float)TEMP3/16;

	   	}
		void Temp4_Funciton(void)
	   	{
			Presence4 = DS18B20_Start_3 ();
			    HAL_Delay (1);
			    DS18B20_Write_3 (0xCC);  // skip ROM
			    DS18B20_Write_3 (0x44);  // convert t
			    HAL_Delay (50);

			    Presence4 = DS18B20_Start_3 ();
			    HAL_Delay(1);
			    DS18B20_Write_3 (0xCC);  // skip ROM
			    DS18B20_Write_3 (0xBE);  // Read Scratch-pad

			    Temp4_byte1 = DS18B20_Read_3();
			    Temp4_byte2 = DS18B20_Read_3();
			    TEMP4=(Temp4_byte2<<8)|Temp4_byte1;
			    Temperature4=(float)TEMP4/16;
	   	}
		void Temp5_Funciton(void)
	   	{
			Presence5 = DS18B20_Start_4 ();
			    HAL_Delay (1);
			    DS18B20_Write_4 (0xCC);  // skip ROM
			    DS18B20_Write_4 (0x44);  // convert t
			    HAL_Delay (50);

			    Presence5 = DS18B20_Start_4 ();
			    HAL_Delay(1);
			    DS18B20_Write_4 (0xCC);  // skip ROM
			    DS18B20_Write_4 (0xBE);  // Read Scratch-pad

			    Temp5_byte1 = DS18B20_Read_4();
			    Temp5_byte2 = DS18B20_Read_4();
			    TEMP5=(Temp5_byte2<<8)|Temp5_byte1;
			    Temperature5=(float)TEMP5/16;
	   	}
		void Temp6_Funciton(void)
	   	{
			 Presence6 = DS18B20_Start_5 ();
		     HAL_Delay (1);
		     DS18B20_Write_5 (0xCC);  // skip ROM
		     DS18B20_Write_5 (0x44);  // convert t
			 HAL_Delay (50);

			 Presence6 = DS18B20_Start_5 ();
			 HAL_Delay(1);
			 DS18B20_Write_5 (0xCC);  // skip ROM
			 DS18B20_Write_5 (0xBE);  // Read Scratch-pad

			 Temp6_byte1 = DS18B20_Read_5();
			 Temp6_byte2 = DS18B20_Read_5();
			 TEMP6=(Temp6_byte2<<8)|Temp6_byte1;
			 Temperature6=(float)TEMP6/16;
	   	}
		void Temp7_Funciton(void)
	   	{
			 Presence7 = DS18B20_Start_6 ();
		     HAL_Delay (1);
			 DS18B20_Write_6 (0xCC);  // skip ROM
			 DS18B20_Write_6 (0x44);  // convert t
			 HAL_Delay (50);

			 Presence7 = DS18B20_Start_6 ();
			 HAL_Delay(1);
			 DS18B20_Write_6 (0xCC);  // skip ROM
			 DS18B20_Write_6 (0xBE);  // Read Scratch-pad

			 Temp7_byte1 = DS18B20_Read_6();
			 Temp7_byte2 = DS18B20_Read_6();
			 TEMP7=(Temp7_byte2<<8)|Temp7_byte1;
			 Temperature7=(float)TEMP7/16;
	   	}
		void Temp8_Funciton(void)
	   	{
			 Presence8 = DS18B20_Start_7 ();
		     HAL_Delay (1);
			 DS18B20_Write_7 (0xCC);  // skip ROM
			 DS18B20_Write_7 (0x44);  // convert t
			 HAL_Delay (50);

			 Presence8 = DS18B20_Start_7 ();
			 HAL_Delay(1);
			 DS18B20_Write_7 (0xCC);  // skip ROM
			 DS18B20_Write_7 (0xBE);  // Read Scratch-pad

			 Temp8_byte1 = DS18B20_Read_7();
			 Temp8_byte2 = DS18B20_Read_7();
			 TEMP8=(Temp8_byte2<<8)|Temp8_byte1;
			 Temperature8=(float)TEMP8/16;

	   	}
	   	void Read_Pins(void)
	   	   	{

	   	          GPIODATA[0]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
	   	   		  GPIODATA[1]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
	   	   		  GPIODATA[2]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
	   	   		  GPIODATA[3]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
	   	   		  GPIODATA[4]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
	   	   		  GPIODATA[5]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
	   	   		  GPIODATA[6]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
	   	   		  GPIODATA[7]=!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
	   	   	}
	   	void SendTempFunction(void)
	   	{
/********************************Send Temp 1***************************/

	   		if(Temperature1>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature1<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature1*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature1>=0 && Temperature1<200 ){
	   		 	  	        a=Temperature1*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/********************************Send Temp 2***************************/
	   		if(Temperature2>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature2<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature2*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature2>=0 && Temperature2<200 ){
	   		 	  	        a=Temperature2*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/********************************Send Temp 3***************************/
	   		if(Temperature3>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature3<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature3*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature3>=0 && Temperature3<200 ){
	   		 	  	        a=Temperature3*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/***********************************Send Temp 4***************************/
	   		if(Temperature4>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature4<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature4*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature4>=0 && Temperature4<200 ){
	   		 	  	        a=Temperature4*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/******************************Send Temp 5*********************************/
	   		if(Temperature4>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature5<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature5*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature5>=0 && Temperature5<200 ){
	   		 	  	        a=Temperature5*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/*****************************Send Temp 6*******************************/
	   		if(Temperature6>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature6<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature6*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature6>=0 && Temperature6<200 ){
	   		 	  	        a=Temperature6*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/**********************Send Temp 7**********************************/
	   		if(Temperature7>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature7<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature7*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature7>=0 && Temperature7<200 ){
	   		 	  	        a=Temperature7*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
/******************************Send Temp 8******************************/
	   		if(Temperature8>4000)
	   		 	           {
	   		 	        	 HAL_UART_Transmit(&huart1,(uint8_t*)"000000",6,HAL_MAX_DELAY);
	   		 	           }
	   		 	        else if(Temperature8<0)
	   		 	  	 	   {
	   		 	  	 		a=Temperature8*(-10);
	   		 	              byte[0]=(a/1000)%10;
	   			 	  	 	  byte[1]=(a/100)%10;
	   			 	  	 	  byte[2]=(a/10)%10;
	   			 	  	 	  byte[3]=a%10;
	   			 	  	 	  sprintf(tempbyte,"1-%d%d,%d",byte[1],byte[2],byte[3]);
	   			 	  	 	  HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   		 	  	 	   else if(Temperature8>=0 && Temperature8<200 ){
	   		 	  	        a=Temperature8*(10);
	   		 	  	        byte[0]=(a/1000)%10;
	   		 	  	        byte[1]=(a/100)%10;
	   		 	  	        byte[2]=(a/10)%10;
	   		 	  	     	byte[3]=a%10;
	   		 	  	     	sprintf(tempbyte,"1%d%d%d,%d",byte[0],byte[1],byte[2],byte[3]);
	   		 	  	     	HAL_UART_Transmit(&huart1, (uint8_t*)tempbyte, strlen(tempbyte),HAL_MAX_DELAY);
	   		 	  	 	   }
	   	}

		 void SendSwtcFunction(void)
		{

		 sprintf(swbyte,"1%d2%d3%d4%d5%d6%d7%d8%d",GPIODATA[0],GPIODATA[1],GPIODATA[2],GPIODATA[3],GPIODATA[4],GPIODATA[5],GPIODATA[6],GPIODATA[7]);
		 HAL_UART_Transmit(&huart1, (uint8_t*)swbyte,16,HAL_MAX_DELAY);

		}



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  HAL_UART_Receive_IT(&huart1, &Rx_data, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //HAL_UART_ErrorCallback(&huart1);
	  HAL_SuspendTick();
	  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	  HAL_ResumeTick();
	  HAL_Delay(500);
	  Read_Pins();
      Temp1_Funciton();
      Temp2_Funciton();
	  Temp3_Funciton();
	  Temp4_Funciton();
	  Temp5_Funciton();
	  Temp6_Funciton();
	  Temp7_Funciton();
	  Temp8_Funciton();

	//  HAL_SuspendTick();
	//  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	 // HAL_ResumeTick();
	  SendTempFunction();
	  SendSwtcFunction();


	  HAL_Delay(1000);





  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 
                           PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB3 PB4 PB5 
                           PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

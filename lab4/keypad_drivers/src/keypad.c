/// <summary>
/// This file contains every implementation of the keypad-related methods
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include <stdio.h>
#include "stm32f4xx.h"
#include "keypad.h"

static GPIO_InitTypeDef  GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_InitStruct;
static TIM_OCInitTypeDef  TIM_OCInitStructure;
static ADC_InitTypeDef adc_init_s;
static ADC_CommonInitTypeDef adc_common_init_s;

char select;    //button pressed
uint8_t readrow = 0;
uint8_t readcol = 0;
int previousValueRow = 0xFF;
int previousValueCol = 0xFF;

/// <summary>
///	Initializes the keypad
/// </summary>
void keypad_init(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = row_bit_0 | row_bit_1 | row_bit_2 | row_bit_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(row_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = col_bit_0 | col_bit_1 | col_bit_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(col_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = col_bit_useless;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(col_GPIO, &GPIO_InitStructure);
	
	GPIO_ResetBits(col_GPIO, col_bit_useless);
}

/// <summary>
///	Read a value from the keypad
/// </summary>
/// <returns>
/// The value read by the keypad
/// </returns>
int keypad_read(void){
	
	uint8_t read = 0;
	//Default returned character is *;
	uint8_t rowIndex = 3;
	uint8_t colIndex = 0;

	set_row_read();
	
	readrow = GPIO_ReadInputDataBit(row_GPIO, row_bit_0) |
		GPIO_ReadInputDataBit(row_GPIO, row_bit_1)<<1 |
		GPIO_ReadInputDataBit(row_GPIO, row_bit_2)<<2 |
		GPIO_ReadInputDataBit(row_GPIO, row_bit_3)<<3;
		
	switch(readrow){
		case 0x0E:		rowIndex = 0;		break;
		case 0x0D:		rowIndex = 1;		break;
		case 0x0B:		rowIndex = 2;		break;
		case 0x07:		rowIndex = 3;		break;
		default : 		rowIndex = 0xFF;  break;
	}
		
	set_col_read();
	
	readcol = GPIO_ReadInputDataBit(col_GPIO, col_bit_0) |
		GPIO_ReadInputDataBit(col_GPIO, col_bit_1)<<1 |
		GPIO_ReadInputDataBit(col_GPIO, col_bit_2)<<2;
	
	if (rowIndex != 0xFF)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_SET);
		while ((GPIO_ReadInputDataBit(col_GPIO, col_bit_0) |
			GPIO_ReadInputDataBit(col_GPIO, col_bit_1)<<1 |
			GPIO_ReadInputDataBit(col_GPIO, col_bit_2)<<2) != 0x07)
		{			
		}
		
		GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET);
	}
	
	switch(readcol){
		case 0x06:		colIndex = 0;		break;
		case 0x05:		colIndex = 1;		break;
		case 0x03:		colIndex = 2;		break;
		default : 		colIndex = 0xFF;  break;
	}
	
	/*if ((previousValueCol == colIndex) || (previousValueRow == rowIndex))
	{
		previousValueCol = colIndex;
		previousValueRow = rowIndex;
		return NO_KEY_PRESSED;
	}*/
		
	/*if (previousValueCol == 0xFF || previousValueRow == 0xFF)
	{
		previousValueCol = colIndex;
		previousValueRow = rowIndex;
		return NO_KEY_PRESSED;
	}*/
	
	/*int colToReturn = previousValueCol;
	int rowToReturn = previousValueRow;
	previousValueCol = colIndex;
	previousValueRow = rowIndex;*/
	
	//return map[rowToReturn][colToReturn];
	if (colIndex == 0xFF || rowIndex == 0xFF)
	{
		return NO_KEY_PRESSED;
	}
	return map[rowIndex][colIndex];
}

/// <summary>
///	Set the pins to read the rows
/// </summary>		
void set_row_read(){
	GPIO_InitStructure.GPIO_Pin = row_bit_0 | row_bit_1 | row_bit_2 | row_bit_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(row_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = col_bit_0 | col_bit_1 | col_bit_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(col_GPIO, &GPIO_InitStructure);
}

/// <summary>
///	Set the pins to read the columns
/// </summary>
void set_col_read(){
	GPIO_InitStructure.GPIO_Pin = row_bit_0 | row_bit_1 | row_bit_2 | row_bit_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(row_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = col_bit_0 | col_bit_1 | col_bit_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(col_GPIO, &GPIO_InitStructure);
	
}

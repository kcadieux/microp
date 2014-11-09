/// <summary>
/// This file contains the method for everything needed by the 7-segment display
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include "segmentDisplay.h"

static TIM_TimeBaseInitTypeDef TIM_InitStruct;
static TIM_OCInitTypeDef  TIM_OCInitStructure;
static ADC_InitTypeDef adc_init_s;
static ADC_CommonInitTypeDef adc_common_init_s;

/// <summary>
///	Initialize the segment display
/// </summary>
void init_segment()
{		
	ADC_CommonInitTypeDef adc_common_init_s;
	ADC_InitTypeDef adc_init_s;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8| 
                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |   
                GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void segments(int digit)
{
	if(digit > 9 || digit < 0)													//The digit must be a value between 0 and 9, inclusively.
		return;
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_1|			//Reset the segments.	
			GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2);
	
	//Chose the right signal mapping.
	switch(digit){
	case 0: GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2); break;
	case 1:	GPIO_SetBits(GPIOB, GPIO_Pin_7|GPIO_Pin_2); break;
	case 2: GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8); break;
	case 3: GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2); break;
	case 4: GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_2); break;
	case 5: GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_2); break;
	case 6: GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_2); break;
	case 7: GPIO_SetBits(GPIOB, GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2); break;
	case 8: GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2); break;
	case 9: GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2|GPIO_Pin_1); break;
	default: break;
	}
}

/// <summary>
///	Display a float on the 7-segment display
/// </summary>		
/// <param_name="output">
/// The float to be displayed
/// </param>
void display(float output)
{
	int outputInt = (int)output;
	int units = outputInt%10;
	int tens = (outputInt%100)/10;
	int hundreds = (outputInt%1000)/100;
	
	int decimals;
	int hundredths;
	
	if (hundreds == 0)
	{
		outputInt = (int)(output*100);			
		decimals = (outputInt%100)/10;
		hundredths = outputInt%10;
	}	
	 
	if  (TIM_select == 0) {     
		GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);       //Close off 7-segment B and C.
		GPIO_SetBits(GPIOB, GPIO_Pin_11);                                       //Turn on 7-segment A.
		if (hundreds != 0)
		{
			segments(hundreds);  
		}
		else if (tens != 0)
		{
			segments(tens);
		}
		else
		{
			segments(units);
			GPIO_SetBits(GPIOB, GPIO_Pin_15);
		}
	}    
			
	if (TIM_select == 1) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);       //Close off 7-segment A and C.
		GPIO_SetBits(GPIOB, GPIO_Pin_12);                                       //Turn on 7-segment B.
		if (hundreds != 0)
		{
			segments(tens);  
		}
		else if (tens != 0)
		{
			segments(units);
			GPIO_SetBits(GPIOB, GPIO_Pin_15);
		}
		else
		{
			segments(decimals);
		}
	}     
			
	if (TIM_select == 2) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);       //Close off 7-segment A and B.
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		if (hundreds != 0)
		{
			segments(units);
		}
		else if (tens != 0)
		{
			segments(decimals);
		}
		else
		{
			segments(hundredths);
		}
	}
		
	if (TIM_select == 3) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);       //Close off 7-segment A and B.
		GPIO_SetBits(GPIOB, GPIO_Pin_14);
		//segments degree symbol
		GPIO_ResetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_1|			//Reset the segments.	
		GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2);
		GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_0);
	}           
}

/// <summary>
///	Display the guessed value on the 7-segment display
/// </summary>		
/// <param_name="output">
/// The float to be outputted
/// </param>
void displayGuessedValue(float output)
{
	int outputInt = (int)(output*100);
	
	//int decimal = (int)(output*10);
	//int outputInt = (int)output;
	//int decimal = (int)(output*10) - outputInt*10;
	//int units = outputInt%10;
	//int tens = (outputInt%100)/10;
	// int hundreds = (outputInt%1000)/100;
	
	int units = (outputInt%1000)/100;
	int tens = (outputInt%10000)/1000;
	int hundreds = (outputInt%100000)/10000;
	int decimals = (outputInt%100)/10;
	int hundredths = outputInt%10;
	
	/*
	if (hundreds == 0)
	{
		outputInt = (int)(output*100);			
		decimals = (outputInt%100)/10;
		hundredths = outputInt%10;
	}
	*/
	 
	if  (TIM_select == 0) {     
		GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);       //Close off 7-segment B and C.
		GPIO_SetBits(GPIOB, GPIO_Pin_11);                                       //Turn on 7-segment A.
		if (hundreds != 0)
		{
			segments(hundreds);  
		}
		else if (tens != 0)
		{
			segments(tens);
		}
		else
		{
			segments(units);
			GPIO_SetBits(GPIOB, GPIO_Pin_15);
		}
	}    
			
	if (TIM_select == 1) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);       //Close off 7-segment A and C.
		GPIO_SetBits(GPIOB, GPIO_Pin_12);                                       //Turn on 7-segment B.
		if (hundreds != 0)
		{
			segments(tens);  
		}
		else if (tens != 0)
		{
			segments(units);
			GPIO_SetBits(GPIOB, GPIO_Pin_15);
		}
		else
		{
			segments(decimals);
		}
	}     
			
	if (TIM_select == 2) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);       //Close off 7-segment A and B.
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		if (hundreds != 0)
		{
			segments(units);
		}
		else if (tens != 0)
		{
			segments(decimals);
		}
		else
		{
			segments(hundredths);
		}
	}
		
	if (TIM_select == 3) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);       //Close off 7-segment A and B.
		GPIO_SetBits(GPIOB, GPIO_Pin_14);
		//segments degree symbol
		GPIO_ResetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_1|			//Reset the segments.	
		GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_2);
		GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_0);
	}           
}


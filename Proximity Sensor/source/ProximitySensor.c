#include <stdio.h>
#include "ProximitySensor.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"


float LPF[20];

float LowPassFilter(float value)
{
	int i;
	float sum = 0.f;
	float average;

	if(LPF[0] == 0){ //Check to see if the first value of the array = 0
		LPF[0] = value; //Store the value in the first index
	}
	else{
		for(int j=20-1; j>=1; j--) 
		{
			LPF [j] = LPF[j-1]; //Otherwise, shift all the values to the right
		}
		LPF[0] = value; //Store the value in the first index in the array
	}
	 
	for (i=0; i<20; i++)
	{
		sum = sum + LPF[i];
	}
	
	average = sum/20; //Returns the average 
	return(average);
}


static void InitADC()
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef adc_init_s;
	ADC_CommonInitTypeDef adc_common_init_s;
	adc_common_init_s.ADC_Mode = ADC_Mode_Independent;
	adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div4;
	adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	
	ADC_CommonInit(&adc_common_init_s); 
	adc_init_s.ADC_Resolution = ADC_Resolution_12b;
	adc_init_s.ADC_ScanConvMode = ENABLE;
	adc_init_s.ADC_ContinuousConvMode = ENABLE;
	adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init_s.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init_s.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc_init_s); 
	
	//Enable Module
	ADC_Cmd(ADC1, ENABLE); 
	 /* Connecting ADC1 Channel 1 to GPIOA pin 1*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC

}


static void InitGPIO()
{
	GPIO_InitTypeDef gpio_init_s;
	
	//For Sensor
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Pin = GPIO_Pin_1;
	gpio_init_s.GPIO_Mode = GPIO_Mode_AN;
	gpio_init_s.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_s.GPIO_OType = GPIO_OType_OD;
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &gpio_init_s);
	//For LEDs
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_s.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOD, &gpio_init_s);	
}

void  InitProximitySensor()
{
	InitADC();
	InitGPIO();
}


void  RequestData()
{
	ADC_SoftwareStartConv(ADC1);
}

int   IsDataReady()
{
	return ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET;
}

float GetData()
{
	uint16_t adcVal;
	ADC_ClearFlag (ADC1, ADC_FLAG_EOC);
	adcVal = ADC_GetConversionValue(ADC1);
	
	/* Do interploation here */
	
	
	return adcVal;
}

float GetFilteredData()
{
	float temp = GetData();
	return LowPassFilter(temp);
}

void 	TurnOffLights()
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void  TurnOnLight(int led)
{
	TurnOffLights();
	switch (led) {
	case 1:
		GPIO_SetBits (GPIOD, GPIO_Pin_12);
		break;
	case 2:
		GPIO_SetBits (GPIOD, GPIO_Pin_13);
		break;
	case 3:
		GPIO_SetBits (GPIOD, GPIO_Pin_14);
		break;
	case 4:
		GPIO_SetBits (GPIOD, GPIO_Pin_15);
		break;
	}
}
#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

/// <summary>
/// Converts a given voltage value read from ADC to 
/// the temperature value in Celsius.
/// </summary>
/// <remarks>
/// Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25
/// Where:
/// -V25 = VSENSE value for 25° C (0.76V, looking at the data sheet)
/// -Avg_Slope = average slope of the temperature vs. 
///  VSENSE curve (2.5mV/Celsius degrees, looking at the data sheet)
/// </remarks>
/// <param_name="voltageValue">
/// The voltage value, in mV 
/// </param>
/// <returns>
/// The temperature, in Celsius degrees
/// </returns>
float ConvertToTemperature(float voltageValue)
{
//	voltageValue *= 3000;
//	
//	voltageValue /= ((2.0e12) -1);
//	voltageValue /= 1000;
//	voltageValue -= 0.760;
//	voltageValue /= 0.0025;
//	voltageValue += 25;
	
	voltageValue *= 3000; // VREF = 3mV
	voltageValue /= 0xfff; //Reading in mV
	voltageValue /= 1000.0; //Reading in Volts
	voltageValue -= 0.760; // Subtract the reference voltage at 25°C
	voltageValue /= .0025; // Divide by slope 2.5mV
	voltageValue += 25.0; // Add the 25°C
	
	
	return voltageValue;
}

/// <summary>
/// Initializes the ADC temperature sensor
/// </summary>
void InitializeAdcTemperatureSensor()
{
	ADC_InitTypeDef adc_init_s;
	ADC_CommonInitTypeDef adc_common_init_s;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //Clock Gating
	adc_common_init_s.ADC_Mode = ADC_Mode_Independent;
	adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div2;
	adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adc_common_init_s); //Initialization
	adc_init_s.ADC_Resolution = ADC_Resolution_12b;
	adc_init_s.ADC_ScanConvMode = DISABLE;
	adc_init_s.ADC_ContinuousConvMode = DISABLE;
	adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init_s.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init_s.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc_init_s); //Initialization
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE); //Enable Module
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC
}

///LD3, LD4, LD5, LD6

int main(){	
	InitializeAdcTemperatureSensor();
	float voltageValue = 0;
	float temperatureValue = 0;
	
	while(1){
		ADC_SoftwareStartConv(ADC1); //Start the conversion
		while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				 ; //Processing the conversion
		voltageValue = ADC_GetConversionValue(ADC1); //Return the converted data
		temperatureValue = ConvertToTemperature(voltageValue);
		printf("%f\n", temperatureValue);
	}
	
	return 0;
}

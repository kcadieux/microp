#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define LED_GREEN      				GPIO_Pin_12
#define LED_ORANGE     				GPIO_Pin_13
#define LED_RED        				GPIO_Pin_14
#define LED_BLUE       				GPIO_Pin_15
#define Motor_Pin							GPIO_Pin_11

#define NUMBER_OF_LED					4
#define ALARM_TEMPERATURE 		60
#define TICK_MULTIPLE 				8
#define FREQUENCY 						50
#define AVERAGE_WINDOW_SIZE 	31

static volatile uint_fast16_t Ticks; // Ticks for the SysTicks
uint_fast32_t AlarmFlag;
uint32_t BufferIndex;
float Buffer[AVERAGE_WINDOW_SIZE];
uint16_t IsBufferFull;

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

void AddValueToBuffer(float temp)
{
	Buffer[BufferIndex % AVERAGE_WINDOW_SIZE] = temp;
	BufferIndex++;
	if (BufferIndex == AVERAGE_WINDOW_SIZE)
	{
		IsBufferFull = 1;
	}
}

float GetAverageTemperature()
{
	if (!IsBufferFull)
	{
		return Buffer[BufferIndex - 1];
	}
	
	int i;
	float sum = 0;
	for (i = 0; i < AVERAGE_WINDOW_SIZE; i++)
	{
		sum += Buffer[i];
	}
	
	return sum / AVERAGE_WINDOW_SIZE;
}

/// <summary>
/// Shuts down all the LEDs
/// </summary>
void CloseLEDs()
{
	GPIO_WriteBit(GPIOD, LED_ORANGE, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_BLUE, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_GREEN, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_RED, Bit_RESET);
}

/// <summary>
/// Initializes the LEDs
/// </summary>
void InitializeGPIO()
{		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB, ENABLE); // Set the clock	
	
	GPIO_InitTypeDef gpio_init_s;
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT; // Set as output
	gpio_init_s.GPIO_Speed = GPIO_Speed_100MHz; // Don't limit slew rate
	gpio_init_s.GPIO_OType = GPIO_OType_PP; // Push-pull
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL; // Not input, don't pull
	
	// Initialization of the LEDs
	gpio_init_s.GPIO_Pin = LED_GREEN;
	GPIO_Init(GPIOD, &gpio_init_s);
	gpio_init_s.GPIO_Pin = LED_ORANGE;
	GPIO_Init(GPIOD, &gpio_init_s);
	gpio_init_s.GPIO_Pin = LED_RED;
	GPIO_Init(GPIOD, &gpio_init_s);
	gpio_init_s.GPIO_Pin = LED_BLUE;
	GPIO_Init(GPIOD, &gpio_init_s);
	
	// Initialization of the servo
	gpio_init_s.GPIO_Pin = Motor_Pin;
	GPIO_Init(GPIOB, &gpio_init_s);
}

/// <summary>
/// Sets the alarm
/// </summary>
void Alarm()
{
	GPIO_WriteBit(GPIOD, LED_ORANGE, Bit_RESET);
	uint32_t alarmState = AlarmFlag % (NUMBER_OF_LED * TICK_MULTIPLE);
	
	if (alarmState <= TICK_MULTIPLE)
	{
		GPIO_WriteBit(GPIOD, LED_RED, Bit_SET);
	}
	else if (alarmState <= 2 * TICK_MULTIPLE)
	{
		GPIO_WriteBit(GPIOD, LED_RED, Bit_RESET);
		GPIO_WriteBit(GPIOD, LED_BLUE, Bit_SET);
	}
	else if (alarmState <= 3 * TICK_MULTIPLE)
	{
		GPIO_WriteBit(GPIOD, LED_BLUE, Bit_RESET);
		GPIO_WriteBit(GPIOD, LED_GREEN, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOD, LED_GREEN, Bit_RESET);
		GPIO_WriteBit(GPIOD, LED_ORANGE, Bit_SET);
	}	
}

/// <summary>
/// Reads the temperature from the ADC
/// </summary>
/// <returns>
/// The temperature in Celsius degrees
/// </returns>
float ReadTemperature()
{
	ADC_SoftwareStartConv(ADC1); //Start the conversion
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Processing the conversion
	ADC_ClearFlag (ADC1, ADC_FLAG_EOC);
	
	return ConvertToTemperature(ADC_GetConversionValue(ADC1));
}

int main(){	
	InitializeAdcTemperatureSensor();	
	InitializeGPIO();	
	SysTick_Config(SystemCoreClock / FREQUENCY); // Systick init 10ms		
	AlarmFlag = 0;
	int someValue = 0;
	
	while(1){
		
		while(!Ticks);
		Ticks = 0;
		
		// Check temperature and go into alarm mode if needed
		AddValueToBuffer(ReadTemperature());
		
		float temperature = GetAverageTemperature();
		printf("%f\n", temperature);
		if (temperature >= ALARM_TEMPERATURE)
		{
			Alarm();
		}
		else
		{			
			CloseLEDs();
		}
		
		// Move the servo to the right position
		GPIO_SetBits(GPIOB, Motor_Pin);
		someValue = 107.84*(temperature*4.25 - 80) + 5809.7;
		for (int i = 0; i < someValue; i++) 
		{
			GPIO_ToggleBits(GPIOB, GPIO_Pin_8);
		}
		GPIO_ResetBits(GPIOB, Motor_Pin);
	}
	
	return 0;
}

/// <summary>
/// Interupt handler for system ticks
/// </summary>
void SysTick_Handler()
{
	AlarmFlag++;
	Ticks = 1;
}

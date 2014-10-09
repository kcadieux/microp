#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define LED_GREEN      							GPIO_Pin_12
#define LED_ORANGE     							GPIO_Pin_13
#define LED_RED        							GPIO_Pin_14
#define LED_BLUE       							GPIO_Pin_15
#define MOTOR_PIN										GPIO_Pin_7

#define NUMBER_OF_LED								4
#define ALARM_TEMPERATURE 					60
#define TICK_MULTIPLE 							8
#define FREQUENCY 									50

// Voltage to temperature conversion constants
#define VREF												3000
#define MV_CONVERSION								0xfff
#define VOLT_IN_MV									1000
#define REFERENCE_VOLTAGE_AT_25C		0.760
#define SLOPE25											0.0025
#define TEMPERATURE_OFFSET					25

static volatile uint_fast16_t Ticks; // Ticks for the SysTicks
uint_fast32_t AlarmFlag; // Flag used for the alarm state

extern void AddValueToWindow(float val);
extern float GetAverageWindow();

static volatile uint_fast16_t Ticks; // Ticks for the SysTicks
uint_fast32_t AlarmFlag;

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
	voltageValue *= VREF; // VREF = 3mV
	voltageValue /= MV_CONVERSION; //Reading in mV
	voltageValue /= VOLT_IN_MV; //Reading in Volts
	voltageValue -= REFERENCE_VOLTAGE_AT_25C; // Subtract the reference voltage at 25°C
	voltageValue /= SLOPE25; // Divide by slope 2.5mV
	voltageValue += TEMPERATURE_OFFSET; // Add the 25°C
	
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
	ADC_CommonInit(&adc_common_init_s); //Initialization
	adc_init_s.ADC_Resolution = ADC_Resolution_12b;
	
	ADC_Init(ADC1, &adc_init_s); //Initialization
	ADC_TempSensorVrefintCmd(ENABLE);
	
	ADC_Cmd(ADC1, ENABLE); //Enable Module
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC
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
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // Set the clock	
	
	GPIO_InitTypeDef gpio_init_s;
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT; // Set as output
	
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
	gpio_init_s.GPIO_Pin = MOTOR_PIN;
	GPIO_Init(GPIOD, &gpio_init_s);
}

/// <summary>
/// Sets the LED alarm, in a circular fashion, from red to orange
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
	AddValueToWindow(ConvertToTemperature(ADC_GetConversionValue(ADC1))); //Add read value to the moving average window
	return GetAverageWindow(); //returns the average of the window
}

/// <summary>
/// Test the circular alarm
/// </summary>
void TestAlarm()
{	
	InitializeGPIO();	
	SysTick_Config(SystemCoreClock / FREQUENCY); // Systick init 10ms			
	AlarmFlag = 0;
	while(1){
		
		while(!Ticks);
		Ticks = 0;
		Alarm();
	}
}

/// <summary>
/// The entry point of the code
/// </summary>
/// <returns>
/// 0 if it terminates
/// </returns>
int main(){		
	InitializeAdcTemperatureSensor();	
	InitializeGPIO();	
	SysTick_Config(SystemCoreClock / FREQUENCY); // Systick init 10ms		
	AlarmFlag = 0;
	int duration = 0;
	int delayInt = 0;
	
	while(1)
	{		
		while(!Ticks);
		
		Ticks = 0;
		
		// Check temperature and go into alarm mode if needed
		float temperature = ReadTemperature();
		printf("%f\n", temperature);
		if (temperature >= ALARM_TEMPERATURE)
		{
			printf("THE ROOF IS ON FIRE\n");
			Alarm();
		}
		else
		{			
			CloseLEDs();
		}
		
		// Move the servo to the right position
		GPIO_SetBits(GPIOD, MOTOR_PIN);
		
		// Not defined as constants since it's a linear function (for easiness, found manually)
		duration = 356.25*(temperature*4.25 -80) + 18106;
		
		for (int i = 0; i < duration; i++) 
		{
			delayInt = 0;
		}
		
		GPIO_ResetBits(GPIOD, MOTOR_PIN);
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

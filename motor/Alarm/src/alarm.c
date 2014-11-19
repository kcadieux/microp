/// <summary>
/// The alarm.c file, containing methods responsible for the alarm
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include "alarm.h"

static GPIO_InitTypeDef  GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_InitStruct;
static TIM_OCInitTypeDef  TIM_OCInitStructure;
static ADC_InitTypeDef adc_init_s;
static ADC_CommonInitTypeDef adc_common_init_s;

TIM_OCInitTypeDef TIM_OCStruct;

/// <summary>
/// Initializes the timer
/// <summary>
/// <remarks>
/// The STM32F4 website was used as a source
/// http://stm32f4-discovery.com/2014/05/stm32f4-stm32f429-discovery-pwm-tutorial/
/// </remarks
void initializeTimer()
{
		TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    
    /* Enable clock for TIM4 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
		// Clock = 42MHz, but has internal PLL : doubles it babe = 84MHz
		// Say we want 10kHz PWM frequency
		// TIM_Period = 84MHz / 10000 - 1 = 8399
    TIM_BaseStruct.TIM_Period = 8399;
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    // Initialize TIM4
    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
    // Start count on TIM4
    TIM_Cmd(TIM4, ENABLE);
}

/// <summary>
/// Initializes the LEDs
/// </summary>
void initializeLEDs()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Clock for GPIOD */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 
    /* Alternating functions for pins */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
    
    /* Set pins */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/// <summary>
/// Initializes the PWM channel
/// </summary>
void initializePWMChannel()
{
	// Mode 2 : clear on compare
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
	
	// Specifies the TIM Output Compare state.
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	
	// Specifies the complementary output polarity.
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	// pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1
		
	// Enabled with pulse = 0 at first
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

/// <summary>
/// Sets the green LED to a given percentage
/// </summary>
/// <param_name="percent">
/// Percent of LED brightness
/// </param>
void setGreen(int percent)
{
	TIM_OCStruct.TIM_Pulse = percent * PERCENT_DUTY_PERIOD;
  TIM_OC1Init(TIM4, &TIM_OCStruct);
}

/// <summary>
/// Sets the orange LED to a given percentage
/// </summary>
/// <param_name="percent">
/// Percent of LED brightness
/// </param>
void setOrange(int percent)
{
	TIM_OCStruct.TIM_Pulse = percent * PERCENT_DUTY_PERIOD;
  TIM_OC2Init(TIM4, &TIM_OCStruct);
}

/// <summary>
/// Sets the blue LED to a given percentage
/// </summary>
/// <param_name="percent">
/// Percent of LED brightness
/// </param>
void setBlue(int percent)
{
	TIM_OCStruct.TIM_Pulse = percent * PERCENT_DUTY_PERIOD;
  TIM_OC4Init(TIM4, &TIM_OCStruct);
}

/// <summary>
/// Sets the red LED to a given percentage
/// </summary>
/// <param_name="percent">
/// Percent of LED brightness
/// </param>
void setRed(int percent)
{
	TIM_OCStruct.TIM_Pulse = percent * PERCENT_DUTY_PERIOD;
  TIM_OC3Init(TIM4, &TIM_OCStruct);
}

/// <summary>
/// Lights up a given LED with a given brightness
/// </summary>
/// <param_name="percent">
/// Percent of LED brightness
/// </param>
/// <param_name="chosenLED">
/// The chosen LED (as defined in the header)
/// </param>
void letThereBeLight (int percent, int chosenLED)
{
	switch(chosenLED)
	{
		case BLUE_LED:
			setRed(0);
			setBlue(percent);
			setGreen(0);
			setOrange(0);
			break;
		case RED_LED:
			setRed(percent);
			setBlue(0);
			setGreen(0);
			setOrange(0);
			break;
		case GREEN_LED:
			setRed(0);
			setBlue(0);
			setGreen(percent);
			setOrange(0);
			break;
		case ORANGE_LED:
			setRed(0);
			setBlue(0);
			setGreen(0);
			setOrange(percent);
			break;
	}
}

/// <summary>
/// Initialize the alarm settings
/// </summary>
void initAlarm()
{
	initializeLEDs();
  initializeTimer();
  initializePWMChannel();
}
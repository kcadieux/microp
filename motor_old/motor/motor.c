/// <summary>
/// The alarm.c file, containing methods responsible for the alarm
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include <stdio.h>
#include "stm32f4xx.h"
#include "motor.h"

static GPIO_InitTypeDef  GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_InitStruct;
static TIM_OCInitTypeDef  TIM_OCInitStructure;
static ADC_InitTypeDef adc_init_s;
static ADC_CommonInitTypeDef adc_common_init_s;

volatile float servo_angle = 0.0;

TIM_OCInitTypeDef TIM_OCStruct;

/// <summary>
/// Initializes the timer
/// <summary>
/// <remarks>
/// The STM32F4 website was used as a source
/// http://stm32f4-discovery.com/2014/05/stm32f4-stm32f429-discovery-pwm-tutorial/
/// </remarks
/*
void initializeTimer()
{
		TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
		// Clock = 42MHz, but has internal PLL : doubles it babe = 84MHz
		// Say we want 10kHz PWM frequency
		// TIM_Period = 84MHz / 10000 - 1 = 8399
		TIM_BaseStruct.TIM_Prescaler = (uint16_t) (((SystemCoreClock / 1000000) / 2) - 1); // Shooting for 1 MHz, (1us)
    TIM_BaseStruct.TIM_Period = 20000 - 1;
    TIM_BaseStruct.TIM_ClockDivision = 0;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    // Initialize TIM4
    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
	
		TIM_ARRPreloadConfig(TIM4, ENABLE);
	
    // Start count on TIM4
    TIM_Cmd(TIM4, ENABLE);
}

/// <summary>
/// Initializes the LEDs
/// </summary>
void initializeLEDs()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
    
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
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
	
	// Specifies the TIM Output Compare state.
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_Pulse = 1500; // Servo Top-Center
	// Specifies the complementary output polarity.
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1
		
	// Enabled with pulse = 0 at first
	//TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	//TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	//TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	//TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	//TIM_OCStruct.TIM_Pulse = 0;
  //TIM_OC1Init(TIM4, &TIM_OCStruct);
	//TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
}
*/

// Integers, or scaled integers (*10) will be more efficient here
 
 // +/- 90.0 degrees
 

 
/**************************************************************************************/
 
void RCC_Configuration(void)
{
  /* --------------------------- System Clocks Configuration -----------------*/
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
 
  /* GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}
 
/**************************************************************************************/
 
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /* Enable the TIM4 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
 
/**************************************************************************************/
 
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  /*-------------------------- GPIO Configuration ----------------------------*/
  /* GPIOD Configuration: Pins 12, 13, 14 and 15 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
  /* Connect TIM4 pins to AF2 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
}
 
/**************************************************************************************/
 
void TIM4_Configuration(void)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 
  /* Time base configuration - SystemCoreClock = 168000000 for 168 MHz board */
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (((SystemCoreClock / 1000000) / 2) - 1); // Shooting for 1 MHz, (1us)
  TIM_TimeBaseStructure.TIM_Period = 20000 - 1; // 1 MHz / 20000 = 50 Hz (20ms)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 
  /* Enable TIM4 Preload register on ARR */
  TIM_ARRPreloadConfig(TIM4, ENABLE);
 
  /* TIM PWM1 Mode configuration: Channel */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1500; // Servo Top-Center
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 
  /* Output Compare PWM1 Mode configuration: Channel1 PD.12 */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
 
  /* Output Compare PWM1 Mode configuration: Channel2 PD.13 */
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
 
  /* Output Compare PWM1 Mode configuration: Channel3 PD.14 */
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
 
  /* Output Compare PWM1 Mode configuration: Channel4 PD.15 */
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
 
  /* TIM Interrupts enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
 
  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}

void initMotor()
{
//	GPIO_InitTypeDef gpio_init_s;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	gpio_init_s.GPIO_Pin = Motor_Pin;
//	GPIO_Init(GPIOB, &gpio_init_s);
	//initializeLEDs();
  //initializeTimer();
  //initializePWMChannel();
	
	    RCC_Configuration();
 
    GPIO_Configuration();
 
    NVIC_Configuration();
 
  TIM4_Configuration();
}

void setMotorAngle(int angle) 
{
	servo_angle = angle;
}

void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
 
    // minimum high of 600 us for -90 degrees, with +90 degrees at 2400 us, 10 us per degree
    //  timer timebase set to us units to simplify the configuration/math
 
    TIM_SetCompare1(TIM4, 1500 + (int)(servo_angle * 10.0f) ); // PD.12
    TIM_SetCompare2(TIM4, 1500 + (int)(servo_angle * 10.0f) ); // PD.13
    TIM_SetCompare3(TIM4, 1500 + (int)(servo_angle * 10.0f) ); // PD.14
    TIM_SetCompare4(TIM4, 1500 + (int)(servo_angle * 10.0f) ); // PD.15
    }
}

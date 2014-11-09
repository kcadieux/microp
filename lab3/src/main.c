/// <summary>
/// This file contains everything related to the lab 3, the keypad and display implementation as well as the accelerometer
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "segmentDisplay.h"
#include "keypad.h"
#include "movingAverageFloat.h"
#include "stm32f4xx_gpio.h"
#include "lis302dl.h"
#include <math.h> 
#include <stdlib.h>     /* abs */
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define MAX_USER_GUESS 5
#define ANGLE_MARGIN 4

#define PI 3.14159265359

#define LED_GREEN      							GPIO_Pin_12
#define LED_ORANGE     							GPIO_Pin_13
#define LED_RED        							GPIO_Pin_14
#define LED_BLUE       							GPIO_Pin_15
#define LED_BATTERY									GPIO_Pin_6


int buffer[3];
float calibratedBuffer[3];
int flag = 0;
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;
static GPIO_InitTypeDef  GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
int TIM_flag = 0;
int TIM_select;

int userInput[3];
int userInputIndex;

float roll_value;
float pitch_value;

int GameGuessState = 0;
float ValueToGuess = -1;
int GameWon = 0;
int GameOver = 0;


static float calibMatrix[12] = {1.01723452075914,	-0.000420062311599107,	0.00193485435221765,
																0.00388909762735978,	1.00489024015481,	-0.0100759161225002,
																0.00258720184597134,	0.00829710539433218,	0.992020257016484,
																-62.2628429437193,	-51.0692244627438,	52.3107895658745};

/// <summary>
///	Enables the timer interrupt
/// </summary>																
void EnableTimerInterrupt()
{
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}			

/// <summary>
///	Initializes the timer 3 and all of its components
/// </summary>		
void init_TIM3()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 40000;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 9;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &timerInitStructure);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	EnableTimerInterrupt();
}
									
/// <summary>
///	Initializes the NVIC
/// </summary>		
void EXTI_NVIC_init()
{
	/* Enable SYSCFG clock: RCC_APB2Periph_SYSCFG correspond to the peripheral handling extInt */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//Set GPIO Int2 Pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/* Connect EXTI Line1 to PE1 pin;  EXTI_PortSourceGPIOE = Line1 */
	SYSCFG_EXTILineConfig(LIS302DL_SPI_INT2_EXTI_PORT_SOURCE, LIS302DL_SPI_INT2_EXTI_PIN_SOURCE);

	/* Configure EXTI Line1 */
	EXTI_InitStructure.EXTI_Line = LIS302DL_SPI_INT2_EXTI_LINE;					//free ext interrupt line.
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line1 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = LIS302DL_SPI_INT2_EXTI_IRQn;							//EXTI Line1 Interrupt 
	
	//Set preemption priority.  First priority list considered.
	//Data sampling in this lab should have highest priority.
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
	
	//Set sub priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					
	
	//Enable the interrupt channel (IRQ channel)
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}//end of EXTI_NVIC_init

/// <summary>
/// Initializes the LEDs
/// </summary>
void InitializeLEDS()
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
	
	// GPIO for external LEDS
	gpio_init_s.GPIO_Pin = LED_BATTERY;
	GPIO_Init(GPIOD, &gpio_init_s);
}

/// <summary>
///	Initializes the accelerometer
/// </summary>		
void init_accel()
{
	/* Set configuration of LIS302DL*/
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	
	//Set the rate to 100 Hz
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	
	//Enable data intake from all three directions
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
	
	//FullScale in the range of +2g to -2g
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);
	
	/* Configure Interrupt control register: enable Click interrupt1 */
  uint8_t ctrl = 0x20;
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);

	
	EXTI_NVIC_init();
	
}

/// <summary>
///	Computes the roll from a given set of coordinates
/// </summary>		
/// <param_name="coords">
/// A set of coordinates (x, y, z)
/// </param>
/// <returns>
/// The computed roll value
/// </returns>
float roll(float* coords)
{
	float tempFloat = 0;
	tempFloat = atan (coords[1]/sqrt(coords[0]*coords[0] + coords[2]*coords[2])) * 180 / PI;
	
	if (coords[2] < 0) {
		if (tempFloat < 0) {
			return -180 - tempFloat;
		} else {
			return 180 - tempFloat;
		}
	} else {
		return tempFloat;
	}
}

/// <summary>
///	Computes the pitch from a given set of coordinates
/// </summary>		
/// <param_name="coords">
/// A set of coordinates (x, y, z)
/// </param>
/// <returns>
/// The computed pitch value
/// </returns>
float pitch(float* coords)
{
	float tempFloat = 0;
	tempFloat = atan (coords[0]/sqrt(coords[1]*coords[1] + coords[2]*coords[2])) * 180 / PI;
	
	if (coords[2] < 0) {
		if (tempFloat < 0) {
			return -180 - tempFloat;
		} else {
			return 180 - tempFloat;
		}
	} else {
		return tempFloat;
	}
}

/// <summary>
///	Initializes all the components for the lab 3
/// </summary>
void init()
{
	InitializeLEDS();
	init_TIM3();
	init_segment();
	init_accel();	
	keypad_init();
}

/// <summary>
///	Clears the user-entered inputs
/// </summary>
void clearInput()
{
	userInput[0] = userInput[1] = userInput[2] = 0;
	userInputIndex = 0;
}

/// <summary>
///	Computes the roll from a given set of coordinates
/// </summary>		
/// <param_name="coords">
/// A set of coordinates (x, y, z)
/// </param>
/// <returns>
/// The user input, as an integer
/// </returns>
int getUserInput()
{
	return (100 * userInput[2]) + (10 * userInput[1]) + userInput[0];
}

/// <summary>
///	Turn on the game over external LEDs
/// </summary>
void gameOverLED()
{
	long z = 9999999;
	while(1)
	{
		z = 9999999;
		GPIO_WriteBit(GPIOD, LED_BATTERY, Bit_SET);
		while(z != 0)
		{
			z--;
		}
		
		z = 9999999;
		GPIO_WriteBit(GPIOD, LED_BATTERY, Bit_RESET);
		while(z != 0)
		{
			z--;
		}
		
		
	}
	
}

/// <summary>
/// Shuts down all the LEDs
/// </summary>
void CloseLEDs()
{
	GPIO_WriteBit(GPIOD, LED_ORANGE | LED_BLUE | LED_GREEN | LED_RED, Bit_RESET);
}

/// <summary>
/// Validates the given input with the chosen angle and take action
/// </summary>
/// <param_name="input">
/// The user input
/// </param>
void validateAngle(int input)
{
	CloseLEDs();
	int someValue = (int)ValueToGuess;
	if (input - someValue >= 4)
	{
		GPIO_WriteBit(GPIOD, LED_RED, Bit_SET);
	}
	else if (input - someValue <= -4)
	{
		GPIO_WriteBit(GPIOD, LED_BLUE, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOD, LED_GREEN, Bit_SET);
		GameWon = 1;
	}
}

/// <summary>
/// Decides of the course of action for the given input and states (acts as a FSM)
/// </summary>
/// <param_name="input">
/// The user input
/// </param>
void processInput(int input)
{
	if (input == POUND)
	{
		// Game on bitch
		if (GameOver || GameWon)
		{
			return;
		}
		
		int userInput = getUserInput();
		
		// input can't be above 180 degrees
		if (userInput > 180)
		{
			clearInput();
			return;
		}
				
		if (GameGuessState == 0)
		{
			ValueToGuess = roll(calibratedBuffer);
			if (ValueToGuess < 0) {
				ValueToGuess *= -1.0;
			}
			GameGuessState++;
			validateAngle(userInput);
			clearInput();
			return;
		}
		
		if (GameGuessState >=1 && GameGuessState < MAX_USER_GUESS)
		{
			GameGuessState++;
			validateAngle(userInput);
			clearInput();
			return;
		}
		
		if (GameGuessState == MAX_USER_GUESS)
		{
			validateAngle(userInput);
			clearInput();
			if (GameWon != 1) {
				gameOverLED();
			}
			return;
		}		
	}
	else if (input == STAR)		
	{
		clearInput();
	}
	else if (input >= 0 && input <= 9)
	{
		if (userInputIndex == 3)
		{
			return;
		}
		
		if (userInputIndex == 2)
		{
			userInput[2] = userInput[1];
			userInput[1] = userInput[0];
		}		
		else if (userInputIndex == 1)
		{
			userInput[1] = userInput[0];
		}
		
		userInput[0] = input;
	
		userInputIndex++;
		return;
	}	
}

/// <summary>
/// Calibrates the accelerometer inputs
/// </summary>
/// <param_name="buff">
/// The accelerometer values
/// </param>
/// <param_name="calibrated">
/// The calibrated values to be outputted
/// </param>
void calibrate(int* buff, float* calibrated) {
	calibrated[0] = (int)buff[0]*calibMatrix[0] + (int)buff[0]*calibMatrix[3] + (int)buff[0]*calibMatrix[6] + calibMatrix[9];
	calibrated[1] = (int)buff[1]*calibMatrix[1] + (int)buff[1]*calibMatrix[4] + (int)buff[1]*calibMatrix[7] + calibMatrix[10];
	calibrated[2] = (int)buff[2]*calibMatrix[2] + (int)buff[2]*calibMatrix[5] + (int)buff[2]*calibMatrix[8] + calibMatrix[11];
}

/// <summary>
/// Filters the accelerometer inputs
/// </summary>
/// <param_name="buff">
/// The accelerometer values
/// </param>
void filter(int* buff)
{
	AddValuesToWindows(buff[0], buff[1], buff[2]);
	buff[0] = GetAverageX();
	buff[1] = GetAverageY();
	buff[2] = GetAverageZ();	
}

/// <summary>
/// The main method, entry point of the code
/// </summary>
int main(){
	
	init();
	while(1)
	{				
		if (TIM_flag == 1) 
		{			
			processInput(keypad_read());
			if (GameWon != 1) {
				display(getUserInput());
			} else {
				displayGuessedValue(ValueToGuess);
			}
			TIM_flag = 0;
		}
				
		if (flag == 1) { 
			filter(buffer);
			calibrate(buffer, calibratedBuffer);
			pitch_value = pitch(calibratedBuffer);
			roll_value = roll(calibratedBuffer);
			
			//printf("%d %d %d\n", (int)buffer[0], (int)buffer[1], (int)buffer[2]);
			//printf("x:%d y:%d z:%d\n", (int)calibratedBuffer[0], (int)calibratedBuffer[1], (int)calibratedBuffer[2]);
			printf("roll:%f\n", roll_value);
			// pitch:%f\n"
			flag = 0;
		}
		
	}
	
	return 0;
}

/// <summary>
/// Handler of the IRQ of EXTI1
/// </summary>
void EXTI1_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line1) != RESET) {
		LIS302DL_ReadACC(buffer);
		EXTI_ClearITPendingBit(EXTI_Line1);
		flag = 1;
	}
}

/// <summary>
/// Handler of the IRQ for the timer 3
/// </summary>
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM_flag = 1;
		TIM_select += 1;
		TIM_select %= 4;
  }
}

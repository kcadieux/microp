#define osObjectsPublic                     // define objects in main module
#define MAX_IS_AWESOME 1

#include <stdio.h>
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "segmentDisplay.h"
#include "keypad.h"
#include "movingAverageFloat.h"
#include "accelerometer.h"
#include "temperature.h"
#include "alarm.h"

#define NO_ALARM_STATE					0
#define ALARM_STATE							1

#define DISPLAY_MODE_TEMP 			0
#define DISPLAY_MODE_ANGLE 			1

#define MAXIMUM_ANGLE						180

#define TEMPERATURE_TRESHOLD 		60

int displayMode = 0;
float rollValue = 0;
float temperatureValue = 0;
int alarmState = NO_ALARM_STATE;
int keyRead = NO_KEY_PRESSED;
int chosenLED = GREEN_LED;
int pwmScale = 0;

// Threads prototype (OH YEAH)
void acc_thread(void const * argument);
void keypad_thread (void const *argument);
void display_thread (void const *argument);
void temp_thread (void const *argument);
void alarm_thread (void const *argument);

// Thread definition baby
osThreadDef(acc_thread, osPriorityNormal, 1, 0);
osThreadDef(keypad_thread, osPriorityBelowNormal, 1, 0);
osThreadDef(display_thread, osPriorityNormal, 1, 0);
osThreadDef(temp_thread, osPriorityNormal, 1, 0);
osThreadDef(alarm_thread, osPriorityNormal, 1, 0);

// Thread ID (NOWAY!)
osThreadId tid_keypad;
osThreadId tid_display;
osThreadId tid_accel;
osThreadId tid_temperature;
osThreadId tid_alarm;

void Blinky_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}

void alarm_thread(void const *argument){
	while(MAX_IS_AWESOME){
		if (temperatureValue >= TEMPERATURE_TRESHOLD)
		{
			alarmState = 1;
		}
		else
		{
			alarmState = 0;
		}
		
		osDelay(5);
	}
}

void acc_thread(void const *argument){
	init_accel(tid_accel);
	while(MAX_IS_AWESOME)
	{		
		
		rollValue = getRoll(tid_accel);
		if (rollValue < 0)
		{
			// Take absolute value
			rollValue *= -1;
		}
		
		// The scale of the impulse (in %)
		pwmScale = (rollValue*100)/MAXIMUM_ANGLE;
		
		letThereBeLight(pwmScale, chosenLED);
		
		osDelay(5);
	}
}

void keypad_thread(void const *argument){
	while(MAX_IS_AWESOME)
	{
		keyRead = keypad_read();
		
		// Mode change
		if (keyRead == POUND)
		{
			displayMode++;
			displayMode %= 2;
			printf("%d\n", keyRead);
		}
		
		// LED change
		if (keyRead >= GREEN_LED && keyRead <= BLUE_LED)
		{
			chosenLED = keyRead;
		}
				
		osDelay(5);
	}
}

void display_thread(void const *argument){
	while(MAX_IS_AWESOME)
	{
		if(displayMode == DISPLAY_MODE_ANGLE)
		{
			//display(888, 0);
			display(rollValue, alarmState);
		}
		else
		{
			//display(888, 0);
			display(temperatureValue, alarmState);
		}
	}		
}

void temp_thread(void const *argument){
	while(MAX_IS_AWESOME)
	{
		temperatureValue = readTemperature();
		//printf("temp = %f\n", temperatureValue);
	}
}

int main (void) {
  osKernelInitialize(); // initialize CMSIS-RTOS
		
  // OMG Initialization be happening
	initTemperatureSensor();
	keypad_init();
	initAlarm();
	init_segment();
	
  // create 'thread' functions that start executing,
	tid_alarm = osThreadCreate(osThread(alarm_thread), NULL);
	tid_accel = osThreadCreate(osThread(acc_thread), NULL);
	tid_temperature = osThreadCreate(osThread(temp_thread), NULL);
	tid_keypad = osThreadCreate(osThread(keypad_thread), NULL);
	tid_display = osThreadCreate(osThread(display_thread), NULL);
	osKernelStart ();                         // start thread execution 
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
        //Set the temperature routine signal.
        osSignalSet(tid_temperature, 0x2);
        //Clear interrupt set bit.
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
  }
}

/// <summary>
/// Handler of the IRQ for the timer 3
/// </summary>
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    osSignalSet(tid_display, 0x4);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		
  }
}

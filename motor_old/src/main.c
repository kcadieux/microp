/// <summary>
/// The main method, holding the threads
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

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
//#include "alarm.h"
#include "motor.h"

#define NO_ALARM_STATE					0
#define ALARM_STATE							1

#define DISPLAY_MODE_TEMP 			0
#define DISPLAY_MODE_ANGLE 			1

#define MAXIMUM_ANGLE						180

#define TEMPERATURE_TRESHOLD 		30

osMutexDef(tempMutex);
osMutexDef(rollMutex);
osMutexDef(alarmMutex);
osMutexDef(ledMutex);
osMutexDef(modeMutex);

//volatile float servo_angle[4] = { 0.0, 0.0, 0.0, 0.0 };
//volatile float servo_angle = 0.0;

int displayMode = 0;
float rollValue = 0;
float temperatureValue = 0;
int alarmState = NO_ALARM_STATE;
int keyRead = NO_KEY_PRESSED;
//int chosenLED = GREEN_LED;
int pwmScale = 0;
int currentAngle = 0.0;

// Mutex ID
osMutexId tempMutex;
osMutexId rollMutex;
osMutexId alarmMutex;
osMutexId ledMutex;
osMutexId modeMutex;

// Threads prototype (OH YEAH)
void acc_thread(void const * argument);
void keypad_thread (void const *argument);
void display_thread (void const *argument);
void temp_thread (void const *argument);
void alarm_thread (void const *argument);
void motor_thread (void const *argument);

// Thread definition baby
osThreadDef(acc_thread, osPriorityNormal, 1, 0);
osThreadDef(keypad_thread, osPriorityNormal, 1, 0);
osThreadDef(display_thread, osPriorityNormal, 1, 0);
osThreadDef(temp_thread, osPriorityNormal, 1, 0);
osThreadDef(alarm_thread, osPriorityNormal, 1, 0);
osThreadDef(motor_thread, osPriorityNormal, 1, 0);

// Thread ID (NOWAY!)
osThreadId tid_keypad;
osThreadId tid_display;
osThreadId tid_accel;
osThreadId tid_temperature;
osThreadId tid_alarm;
osThreadId tid_motor;

/// <summary>
/// The alarm thread
/// </summary>
void alarm_thread(void const *argument){
	//initAlarm();
	while(MAX_IS_AWESOME){
		int alarm = 0;
		if (temperatureValue >= TEMPERATURE_TRESHOLD)
		{
			alarm = 1;
		}
		else
		{
			alarm = 0;
		}
		
		osMutexWait(alarmMutex, 0);
		alarmState = alarm;
		osMutexRelease(alarmMutex);
		
		osDelay(5);
	}
}


/// <summary>
/// The acceleration thread
/// </summary>
void acc_thread(void const *argument){
	init_accel(tid_accel);
	while(MAX_IS_AWESOME)
	{		
		osMutexWait(rollMutex, osWaitForever);
		rollValue = getRoll(tid_accel);
		if (rollValue < 0)
		{
			// Take absolute value
			rollValue *= -1;
		}		
		
		// The scale of the impulse (in %)
		pwmScale = (rollValue*100)/MAXIMUM_ANGLE;
		osMutexRelease(rollMutex);
		
		//letThereBeLight(pwmScale, chosenLED);		
	}
}

/// <summary>
/// The keypad thread
/// </summary>
void keypad_thread(void const *argument){
	keypad_init(tid_keypad);
	while(MAX_IS_AWESOME)
	{
		keyRead = keypad_read();
		
		// Mode change
		if (keyRead == POUND)
		{
			osStatus status = osMutexWait(modeMutex, 0);
			displayMode++;
			displayMode %= 2;
			osMutexRelease(modeMutex);			
		}
		
		// LED change
		//if (keyRead >= GREEN_LED && keyRead <= BLUE_LED)
		//{
		//	osMutexWait(ledMutex, 0);
		//	chosenLED = keyRead;
		//	osMutexRelease(ledMutex);
		//}
		if (keyRead == 4)
		{
			currentAngle -= 5;
			setMotorAngle(currentAngle);
		}
		if (keyRead == 5)
		{
			currentAngle += 5;
			setMotorAngle(currentAngle);
		}
				
		osDelay(25);
	}
}

/// <summary>
/// The display thread
/// </summary>
void display_thread(void const *argument){
	init_segment();
	while(MAX_IS_AWESOME)
	{
		osMutexWait(modeMutex, 0);
		int mode = displayMode;
		osMutexRelease(modeMutex);
			
		osMutexWait(alarmMutex, 2);
		int alarm = alarmState;
		osMutexRelease(alarmMutex);

		osMutexWait(rollMutex, 2);
		float roll = rollValue;
		osMutexRelease(rollMutex);
		
		osMutexWait(tempMutex, 2);
		float temperature = temperatureValue;
		osMutexRelease(tempMutex);
		
		if(mode == DISPLAY_MODE_ANGLE)
		{
			display(roll, alarm);
		}
		else
		{
			display(temperature, alarm);
		}
		
	}		
}

/// <summary>
/// The temperature thread
/// </summary>
void temp_thread(void const *argument){
	initTemperatureSensor();
	while(MAX_IS_AWESOME)
	{
		osMutexWait(tempMutex, 2);
		temperatureValue = readTemperature();
		osMutexRelease(tempMutex);
	}
}

/// <summary>
/// Initialize the mutual exclusion
/// </summary>
void initMutex()
{
	tempMutex = osMutexCreate(osMutex(tempMutex));
	rollMutex = osMutexCreate(osMutex(rollMutex));
	alarmMutex = osMutexCreate(osMutex(alarmMutex));
	ledMutex = osMutexCreate(osMutex(ledMutex));
	modeMutex = osMutexCreate(osMutex(modeMutex));
}

void motor_thread(void const *argument)
{
	initMotor();
}

/// <summary>
/// The entry point of the code
/// </summary>
int main (void) {
  osKernelInitialize(); // initialize CMSIS-RTOS
		
  // OMG Initialization be happening
	initMutex();
	initMotor();
	//tid_motor = osThreadCreate(osThread(motor_thread), NULL);
  // create 'thread' functions that start executing,
	//tid_alarm = osThreadCreate(osThread(alarm_thread), NULL);
	//tid_accel = osThreadCreate(osThread(acc_thread), NULL);
	//tid_temperature = osThreadCreate(osThread(temp_thread), NULL);
	tid_keypad = osThreadCreate(osThread(keypad_thread), NULL);
	//tid_display = osThreadCreate(osThread(display_thread), NULL);
	osKernelStart ();                         // start thread execution 
}

/// <summary>
/// The IRQ handler for timer 2
/// </summary>
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
/// The IRQ handler for timer 3
/// </summary>
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    osSignalSet(tid_display, 0x4);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		
  }
}

/// <summary>
/// The IRQ handler for timer 5
/// </summary>
void TIM5_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
  {
    osSignalSet(tid_keypad, 0x5);
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);		
  }
}



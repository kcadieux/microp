#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "osObjects.h"   

#define FULL_DUTY_PERIOD 8399
#define PERCENT_DUTY_PERIOD 84
#define MAXIMUM_ANGLE 150

#define Motor_Pin					GPIO_Pin_11
#define Default_Angle -90

#define SWEEP_START_SIGNAL  1
#define MOTOR_TICK_SIGNAL   2  

typedef struct 	
{
	int angle;
	int distance;
} position;

int  GetSweepIsActive(void);
void ToggleSweepIsActive(void);
void initSweepTIM(void);
void sweep180(position * pos);
void zerosweep(void);

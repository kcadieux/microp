#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "osObjects.h"   

#define FULL_DUTY_PERIOD 8399
#define PERCENT_DUTY_PERIOD 84

#define GREEN_LED 							1
#define RED_LED 								2
#define ORANGE_LED 							3
#define BLUE_LED 								4

static const int STD_ALARM_TIM_PERIOD = 10000;

extern void initAlarm(void);
extern void letThereBeLight (int percent, int chosenLED);

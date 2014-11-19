
#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "lis302dl.h"
#include <math.h> 
#include <stdlib.h>
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "movingAverageFloat.h"
#include "osObjects.h"

#define PI 3.14159265359

static float calibMatrix[12] = {1.01723452075914,	-0.000420062311599107,	0.00193485435221765,
																0.00388909762735978,	1.00489024015481,	-0.0100759161225002,
																0.00258720184597134,	0.00829710539433218,	0.992020257016484,
																-62.2628429437193,	-51.0692244627438,	52.3107895658745};

																
extern float getRoll();
extern void init_accel(osThreadId tid_accel);																
extern void EXTI1_IRQHandler(void);
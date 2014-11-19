/// <summary>
/// This header contains every declarations needed by the segment display
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

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
#include "osObjects.h"   

#define STABLE_STATE		0
#define FLASH_STATE 		1

/*
#define SEGMENT_PIN_A GPIO_Pin_7
#define SEGMENT_PIN_B GPIO_Pin_8
#define SEGMENT_PIN_C GPIO_Pin_2
#define SEGMENT_PIN_D GPIO_Pin_1
#define SEGMENT_PIN_E GPIO_Pin_5
#define SEGMENT_PIN_F GPIO_Pin_0
#define SEGMENT_PIN_G GPIO_Pin_4

// 12 to 14 for transistor control.
#define TRANSISTOR_PIN_0 GPIO_Pin_12
#define TRANSISTOR_PIN_1 GPIO_Pin_13
#define TRANSISTOR_PIN_2 GPIO_Pin_14

//Pin 15 assigned to the power 0 decimal's dot segment.
#define DOT_PIN_0 GPIO_Pin_15
*/

/// <summary>
///	Initialize the segment display
/// </summary>
extern void init_segment(void);

/// <summary>
///	Display a float on the 7-segment display
/// </summary>		
/// <param_name="output">
/// The float to be displayed
/// </param>
/// <param_name="mode">
/// The mode of the display (STABLE or FLASH)
/// </param>
extern void display(float output, int mode);

/// <summary>
///	Display the guessed value on the 7-segment display
/// </summary>		
/// <param_name="output">
/// The float to be outputted
/// </param>
extern void displayGuessedValue(float output);

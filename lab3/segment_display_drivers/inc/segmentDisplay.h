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


extern int TIM_select;

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
extern void display(float output);

/// <summary>
///	Display the guessed value on the 7-segment display
/// </summary>		
/// <param_name="output">
/// The float to be outputted
/// </param>
extern void displayGuessedValue(float output);

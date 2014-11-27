/// <summary>
/// This file contains the declarations related to the obtention of a moving
/// window average, in float. To change the window size, simply modify
/// the AVERAGE_WINDOW_SIZE constant.
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>
#include <stdio.h>
#include "stm32f4xx.h"

#define AVERAGE_SINGLE_SIZE 	100
#define SAMPLE_SINGLE_SIZE 	5

uint16_t IsBaselineBufferFull(void);

void AddValueToWindowBaseline(int val);
void AddValueToWindowSample(int val);

float GetAverageWindowBaseline(void);
float GetAverageWindowSample(void);

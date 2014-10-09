/// <summary>
/// This file contains everything related to the obtention of a moving
/// window average, in float. To change the window size, simply modify
/// the AVERAGE_WINDOW_SIZE constant.
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include <stdio.h>
#include "stm32f4xx.h"

#define AVERAGE_WINDOW_SIZE 	31

uint32_t BufferIndex;
float Buffer[AVERAGE_WINDOW_SIZE];
uint16_t IsBufferFull;

/// <summary>
/// Adds a given value to the window to average
/// </summary>
/// <param_name="val">
/// The value to be added to the window
/// </param>
void AddValueToWindow(float val)
{
	Buffer[BufferIndex % AVERAGE_WINDOW_SIZE] = val;
	BufferIndex++;
	if (BufferIndex == AVERAGE_WINDOW_SIZE)
	{
		IsBufferFull = 1;
	}
}

/// <summary>
/// Gets the average of the window of values
/// </summary>
/// <returns>
/// The average of the window of values
/// </returns>
float GetAverageWindow()
{
	if (!IsBufferFull)
	{
		return Buffer[BufferIndex - 1];
	}
	
	int i;
	float sum = 0;
	for (i = 0; i < AVERAGE_WINDOW_SIZE; i++)
	{
		sum += Buffer[i];
	}
	
	return sum / AVERAGE_WINDOW_SIZE;
}

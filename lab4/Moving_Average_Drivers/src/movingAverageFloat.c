/// <summary>
/// This file contains everything related to the obtention of a moving
/// window average, in float. 
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include <stdio.h>
#include "stm32f4xx.h"
#include "movingAverageFloat.h"

static uint32_t BufferBaselineIndex;
static uint32_t BufferSampleIndex;
static int BufferBaseline[AVERAGE_SINGLE_SIZE];
static int BufferSample[SAMPLE_SINGLE_SIZE];
static uint16_t IsBufferBaselineFull;
static uint16_t IsBufferSampleFull;

uint16_t IsBaselineBufferFull()
{
	return IsBufferBaselineFull;
}

/// <summary>
/// Adds a given value to the window to average
/// </summary>
/// <param_name="val">
/// The value to be added to the window
/// </param>
void AddValueToWindowBaseline(int val)
{
	BufferBaseline[BufferBaselineIndex % AVERAGE_SINGLE_SIZE] = val;
	BufferBaselineIndex++;
	if (BufferBaselineIndex == AVERAGE_SINGLE_SIZE)
	{
		IsBufferBaselineFull = 1;
	}
}

/// <summary>
/// Adds a given value to the window to average
/// </summary>
/// <param_name="val">
/// The value to be added to the window
/// </param>
void AddValueToWindowSample(int val)
{
	BufferSample[BufferSampleIndex % SAMPLE_SINGLE_SIZE] = val;
	BufferSampleIndex++;
	if (BufferSampleIndex == SAMPLE_SINGLE_SIZE)
	{
		IsBufferSampleFull = 1;
	}
}

/// <summary>
/// Gets the average of the window of values
/// </summary>
/// <returns>
/// The average of the window of values
/// </returns>
float GetAverageWindowBaseline()
{
	int i;
	int sum;
	
	if (!IsBufferBaselineFull)
	{
		float notFullSum = 0;
		for (i = 0; i < BufferBaselineIndex; i++) {
			notFullSum += BufferBaseline[i];
		}
		return notFullSum / (float)(BufferBaselineIndex);
	}
	
	sum = 0;
	for (i = 0; i < AVERAGE_SINGLE_SIZE; i++)
	{
		sum += BufferBaseline[i];
	}
	
	return sum / (float)AVERAGE_SINGLE_SIZE;
}

/// <summary>
/// Gets the average of the window of values
/// </summary>
/// <returns>
/// The average of the window of values
/// </returns>
float GetAverageWindowSample()
{
	int i;
	int sum;
	
	if (!IsBufferSampleFull)
	{
		float notFullSum = 0;
		for (i = 0; i < BufferSampleIndex; i++) {
			notFullSum += BufferSample[i];
		}
		return notFullSum / (float)(BufferSampleIndex);
	}
	
	sum = 0;
	for (i = 0; i < SAMPLE_SINGLE_SIZE; i++)
	{
		sum += BufferSample[i];
	}
	
	return sum / (float)SAMPLE_SINGLE_SIZE;
}

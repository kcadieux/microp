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

uint32_t BufferIndex;
float Buffer[AVERAGE_SINGLE_SIZE];
uint16_t IsBufferFull;
int BufferX[AVERAGE_COORDS_SIZE];
int BufferY[AVERAGE_COORDS_SIZE];
int BufferZ[AVERAGE_COORDS_SIZE];
uint32_t CoordsIndex;
uint16_t AreCoordsFull;

/// <summary>
/// Adds a given value to the window to average
/// </summary>
/// <param_name="val">
/// The value to be added to the window
/// </param>
void AddValueToWindow(float val)
{
	Buffer[BufferIndex % AVERAGE_SINGLE_SIZE] = val;
	BufferIndex++;
	if (BufferIndex == AVERAGE_SINGLE_SIZE)
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
		float notFullSum = 0;
		for (int i = 0; i < BufferIndex; i++) {
			notFullSum += Buffer[i];
		}
		return notFullSum / (BufferIndex);
	}
	
	int i;
	float sum = 0;
	for (i = 0; i < AVERAGE_SINGLE_SIZE; i++)
	{
		sum += Buffer[i];
	}
	
	return sum / AVERAGE_SINGLE_SIZE;
}

/// <summary>
///	Add values to the coordinate windows
/// </summary>		
/// <param_name="x">
/// The x coordinate
/// </param>
/// <param_name="y">
/// The y coordinate
/// </param>
/// <param_name="z">
/// The z coordinate
/// </param>
void AddValuesToWindows(int x, int y, int z)
{
	BufferX[CoordsIndex % AVERAGE_COORDS_SIZE] = x;
	BufferY[CoordsIndex % AVERAGE_COORDS_SIZE] = y;
	BufferZ[CoordsIndex % AVERAGE_COORDS_SIZE] = z;
	
	CoordsIndex++;
	if (CoordsIndex == AVERAGE_COORDS_SIZE)
	{
		AreCoordsFull = 1;
	}
}

/// <summary>
///	Computes the average for the x coordinate
/// </summary>
/// <returns>
/// The average for the x coordinate
/// </returns>
int GetAverageX(void)
{
	if (!AreCoordsFull)
	{
		int notFullSum = 0;
		for (int i = 0; i < CoordsIndex; i++) {
			notFullSum += BufferX[i];
		}
		return notFullSum / (CoordsIndex);
	}
	
	int i;
	int sum = 0;
	for (i = 0; i < AVERAGE_COORDS_SIZE; i++)
	{
		sum += BufferX[i];
	}
	
	return sum / AVERAGE_COORDS_SIZE;
}

/// <summary>
///	Computes the average for the y coordinate
/// </summary>
/// <returns>
/// The average for the y coordinate
/// </returns>
int GetAverageY(void)
{
	if (!AreCoordsFull)
	{
		int notFullSum = 0;
		for (int i = 0; i < CoordsIndex; i++) {
			notFullSum += BufferY[i];
		}
		return notFullSum / (CoordsIndex);
	}
	
	int i;
	int sum = 0;
	for (i = 0; i < AVERAGE_COORDS_SIZE; i++)
	{
		sum += BufferY[i];
	}
	
	return sum / AVERAGE_COORDS_SIZE;
}

/// <summary>
///	Computes the average for the z coordinate
/// </summary>
/// <returns>
/// The average for the z coordinate
/// </returns>
int GetAverageZ(void)
{
	if (!AreCoordsFull)
	{
		int notFullSum = 0;
		for (int i = 0; i < CoordsIndex; i++) {
			notFullSum += BufferZ[i];
		}
		return notFullSum / (CoordsIndex);
	}
	
	int i;
	int sum = 0;
	for (i = 0; i < AVERAGE_COORDS_SIZE; i++)
	{
		sum += BufferZ[i];
	}
	
	return sum / AVERAGE_COORDS_SIZE;
}

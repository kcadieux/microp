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

#define AVERAGE_SINGLE_SIZE 	5

#define AVERAGE_COORDS_SIZE 	61

#define NUMBER_OF_BUFFERS			4

void AddValueToWindow(float val, int index);
float GetAverageWindow(int index);

void AddValuesToWindows(int x, int y, int z);
int GetAverageX(void);
int GetAverageY(void);
int GetAverageZ(void);

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

#define AVERAGE_SINGLE_SIZE 	31

#define AVERAGE_COORDS_SIZE 	61

extern void AddValueToWindow(float val);
extern float GetAverageWindow(void);

extern void AddValuesToWindows(int x, int y, int z);
extern int GetAverageX(void);
extern int GetAverageY(void);
extern int GetAverageZ(void);

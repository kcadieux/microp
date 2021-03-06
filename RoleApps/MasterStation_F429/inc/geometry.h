#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f4xx.h"  

#define MAX_RADII 4
#define PRESENT 1
#define MAX_X 1000
#define MAX_Y 1000

#define STATION0_X 0
#define STATION0_Y 0

#define STATION1_X 1000
#define STATION1_Y 0

#define STATION2_X 1000
#define STATION2_Y 1000

#define STATION3_X 0
#define STATION3_Y 1000

#define RADIUS_OFFSET 50

#define RSSI_AT_1M -67
#define PATH_LOSS_EXPONENT 2.5

void UpdateRadius(float radius, int radiusIndex);

void ComputeCooordinates(void);

float getXcoordinate(void);

float getYcoordinate(void);

float convertRSSIToDistance(float rssi);

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "osObjects.h"   
#include "movingAverageFloat.h"


// Voltage to temperature conversion constants
#define VREF												3000
#define MV_CONVERSION								0xfff
#define VOLT_IN_MV									1000
#define REFERENCE_VOLTAGE_AT_25C		0.760
#define SLOPE25											0.0025
#define TEMPERATURE_OFFSET					25

extern void initTemperatureSensor();
extern float readTemperature();

#ifndef __ProximitySensor__H
#define __ProximitySensor__H

#include <stdint.h>

/* Initializing the Proximity Sensor*/
void  InitProximitySensor(); //Initialize Both GPIO and ADC
static void InitADC();
static void InitGPIO();

float LowPassFilter(float value);

/*Data Collection */ 
void  RequestData(); //Request the ADC to start converting Data
int   IsDataReady(); //ADC sets a flag when data is ready
float GetData();		//Get Unfiltered Data 
float GetFilteredData(); //Gets Data post filtration

/* LEDs */
void 	TurnOffLights();
void  TurnOnLight(int led);

#endif

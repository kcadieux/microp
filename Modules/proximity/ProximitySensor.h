#ifndef __ProximitySensor__H
#define __ProximitySensor__H

#include <stdint.h>

/* Initializing the Proximity Sensor*/
void  InitProximitySensor(void); //Initialize Both GPIO and ADC
static void InitADC(void);
static void InitGPIO(void);

float LowPassFilter(float value);

/*Data Collection */ 
void  RequestData(void); //Request the ADC to start converting Data
int   IsDataReady(void); //ADC sets a flag when data is ready
float GetData(void);		//Get Unfiltered Data 
float GetFilteredData(void); //Gets Data post filtration

#endif

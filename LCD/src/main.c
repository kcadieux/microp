/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "lcd.h"

#include <stdio.h>
#include <string.h>

void lcdDisplay(void const *argument)
{
	//starWarsIntro();
	//osDelay(400);
	
	clearBackground();
	initCoords();
	initMap();
	updateCoords(220, 430);
	osDelay(1000);
	updateCoords(420, 430);
	osDelay(1000);
	updateCoords(620, 430);
	while(1);
	
}

osThreadDef(lcdDisplay, osPriorityNormal, 1, 0);

// ID for theads
osThreadId lcdDisplay_thread;

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialize peripherals here
	/* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC controler */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer as the current layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
  // create 'thread' functions that start executing,
	lcdDisplay_thread = osThreadCreate(osThread(lcdDisplay), NULL);
	
	// start thread execution 
	osKernelStart ();
}



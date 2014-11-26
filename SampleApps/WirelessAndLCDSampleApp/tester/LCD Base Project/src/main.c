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
#include "background16bpp.h"
#include "wireless.h"

#include <stdio.h>
#include <string.h>


static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--)
  {
  }
}

char RSSI[7][10];

void SetRssi(int rssi, int rssiNb)
{
	sprintf(RSSI[rssiNb], "   %d", rssi);
  LCD_DisplayStringLine(LINE(rssiNb*2), (uint8_t*)RSSI[rssiNb]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief    Illustartes a simple shape draw and fill, and string dsiplay
  * @function This function draws concentrated colour-filled circles. It also draw a square and a triangle. Some text at two
              different font sizes is displayed.
  * @param    None
  * @retval   None
  */

void example_1a(void const *argument){
	/* Clear the LCD */ 
    LCD_Clear(LCD_COLOR_WHITE);
	
	  
    LCD_SetFont(&Font12x12);

    LCD_DisplayStringLine(LINE(1), (uint8_t*)"          RSSI 1 ");
    LCD_DisplayStringLine(LINE(3), (uint8_t*)"          RSSI 2 ");
    LCD_DisplayStringLine(LINE(5), (uint8_t*)"          RSSI 3 ");
    LCD_DisplayStringLine(LINE(7), (uint8_t*)"          RSSI 4 ");
    LCD_DisplayStringLine(LINE(9), (uint8_t*)"          RSSI 5 ");
    LCD_DisplayStringLine(LINE(11), (uint8_t*)"          RSSI 6 ");
    LCD_DisplayStringLine(LINE(13), (uint8_t*)"          RSSI 7 ");
	
	while(1){
		
		SetRssi(30, 1);
	}
}

void DisplayRSSIThread(void const *argument)
{
	int rssi_db;
	
	/* Clear the LCD */ 
	LCD_Clear(LCD_COLOR_WHITE);

	
	LCD_SetFont(&Font12x12);

	LCD_DisplayStringLine(LINE(1), (uint8_t*)"  RSSI 1 ");
	LCD_DisplayStringLine(LINE(3), (uint8_t*)"  RSSI 2 ");
	LCD_DisplayStringLine(LINE(5), (uint8_t*)"  RSSI 3 ");
	LCD_DisplayStringLine(LINE(7), (uint8_t*)"  RSSI 4 ");
	LCD_DisplayStringLine(LINE(9), (uint8_t*)"  RSSI 5 ");
	LCD_DisplayStringLine(LINE(11), (uint8_t*)"  RSSI 6 ");
	LCD_DisplayStringLine(LINE(13), (uint8_t*)"  RSSI 7 ");
	
	while (1)
	{
		uint8_t packet[WLESS_PACKET_SIZE];
		WLESS_ReceivePacketVerified(packet);
		rssi_db = WLESS_GetLatestDecibelRSSI();
		
		SetRssi(rssi_db, packet[0]);
		
		osDelay(10);
	}
}


osThreadDef(DisplayRSSIThread, osPriorityNormal, 1, 0);

// ID for theads
osThreadId example_1a_thread;

/*
 * main: initialize and start the system
 */
int main (void) {
	
	WLESS_InitTypeDef wless_init_s;
	
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
	
	
	wless_init_s.address = 1;	
	WLESS_Init(&wless_init_s);
	
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	/*******************************************************
	         Uncomment the example you want to see
	example_1a: Simple shape draw, fill and text display
	example_1b: bitmap image display
	example_1c: Simple animation
	********************************************************/
	
	example_1a_thread = osThreadCreate(osThread(DisplayRSSIThread), NULL);
	//example_1b_thread = osThreadCreate(osThread(example_1b), NULL);
	//example_1c_thread = osThreadCreate(osThread(example_1c), NULL);
	
	osKernelStart ();                         // start thread execution 
}



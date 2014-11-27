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
#include "roles.h"
#include "geometry.h"
#include "movingAverageFloat.h"

#include <stdio.h>
#include <string.h>


static char RSSI[7][10];
static unsigned int ticks = 0;
static const int NB_SLAVE_STATIONS = 2;

void SetRssi(float rssi, int rssiNb)
{
	sprintf(RSSI[rssiNb], "   %f", rssi);
  LCD_DisplayStringLine(LINE(rssiNb*2), (uint8_t*)RSSI[rssiNb]);
}


void DisplayRSSIThread(void const *argument)
{
	signed char rssi_db;
	uint8_t packet[WLESS_PACKET_SIZE];
	
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
		int i;
		unsigned int start = ticks;
		unsigned int duration_ms;
		
		if (WLESS_ReceivePacketVerified(ROLES_Address_BADGE_BROADCAST_RECEIVER, packet) != WLESS_StatusCode_RX_SUCCESS) continue;
		if (packet[0] != ROLES_PacketType_BADGE_BROADCAST) continue;
		
		rssi_db = WLESS_GetLatestDecibelRSSI();
		//AddValueToWindow(rssi_db, 1);
		SetRssi(rssi_db, 1);
		//SetRssi(rssi_db, 1);
		
		
		for (i=1; i<=NB_SLAVE_STATIONS; ++i) {
			if (WLESS_ReceivePacketVerified(ROLES_Address_SLAVE_RSSI_RECEIVER_BASE_ADDR + i, packet) != WLESS_StatusCode_RX_SUCCESS) continue;
			if (packet[0] != ROLES_PacketType_STATION_RSSI) continue;
			
			rssi_db = packet[1];
			//AddValueToWindow(rssi_db, i+1);
			SetRssi(rssi_db, i+1);
		}
		
		duration_ms = ticks - start;
		
		/*
		if (duration_ms > 20)
		{
			printf("Long delay...\n");
		}
		*/
	}
}


static const uint32_t PRESCALER = 10000;
static const uint32_t PERIOD = 9;


void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
	++ticks;
}

static void InitMainTimer()
{
	GPIO_InitTypeDef	gpio_init_s;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef 				NVIC_InitStructure;
	
	// Enable the TIM2 gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	 
	// TIM3 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	// Time base configuration 
	TIM_TimeBaseStructure.TIM_Period = PERIOD - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PRESCALER - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	// TIM IT enable
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	// TIM3 enable counter
	TIM_Cmd(TIM3, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s.GPIO_OType = GPIO_OType_PP;
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_s.GPIO_Pin = GPIO_Pin_12;
	
	GPIO_Init(GPIOD, &gpio_init_s);
}


osThreadDef(DisplayRSSIThread, osPriorityNormal, 1, 0);

// ID for theads
osThreadId example_1a_thread;

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
	
  //Init wireless communication module	
	WLESS_Init();
	InitMainTimer();
	
	
	example_1a_thread = osThreadCreate(osThread(DisplayRSSIThread), NULL);
	
	osKernelStart ();                         // start thread execution 
}



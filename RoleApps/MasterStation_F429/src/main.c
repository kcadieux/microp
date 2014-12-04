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
#include "motor.h"
#include "sweep.h"
#include "ProximitySensor.h"
#include "lcd.h"

#include <stdio.h>
#include <string.h>

#define MAXIMUM_ANGLE						180

static void DisplayRSSIThread(void const *argument);
static void sweep_thread(void const *argument);

int currentAngle = 0.0;

static char RSSI[7][10];
static unsigned int ticks = 0;

static const uint32_t PRESCALER = 10000;
static const uint32_t PERIOD = 9;

osThreadDef(ROLES_MasterStationThread, osPriorityNormal, 1, 0);
osThreadDef(DisplayRSSIThread, osPriorityNormal, 1, 0);
osThreadDef(sweep_thread, osPriorityNormal, 1, 0);

static int ObjectAngle = 0;
static int ObjectDistance = 0;

// ID for theads
osThreadId masterStationThreadId;
osThreadId example_1a_thread;
osThreadId tid_sweep;

void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
	++ticks;
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
			ticks++;
			//Set the temperature routine signal.
			GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
			if (ticks == 50) {
				//
				ticks = 0;
			}
			RequestData();	
			osSignalSet(tid_sweep, MOTOR_TICK_SIGNAL);
			//Clear interrupt set bit.
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
  }
}

void SetRssi(float rssi, int rssiNb)
{
	sprintf(RSSI[rssiNb], "   %f", rssi);
  LCD_DisplayStringLine(LINE(rssiNb*2), (uint8_t*)RSSI[rssiNb]);
}


void DisplayRSSIThread(void const *argument)
{
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
		osDelay(100);
	}
}

#define PI 3.14159265

void updatePosition(position * pos)
{
	double angleRadian = pos->angle * PI / 180.0;
	updateCoords(150 + (cos(angleRadian) * pos->distance), sin(angleRadian) * pos->distance);
}

void sweep_thread(void const *argument)
{
	position pos;
	
	clearBackground();
	
	starWarsIntro();
	
	
	initCoords();
	initMap();
	
	zerosweep();
	while(1)
	{
		osSignalWait(SWEEP_START_SIGNAL, osWaitForever);
		setAlarm(1);
		while (GetSweepIsActive())
		{
			sweep180(&pos);
			updatePosition(&pos);
			osSignalSet(osThreadGetId(), SWEEP_START_SIGNAL);
		}
		setAlarm(0);
		osSignalClear(osThreadGetId(), SWEEP_START_SIGNAL);
	}
}


static void InitMainTimer()
{
	GPIO_InitTypeDef	gpio_init_s;
	
	/*
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
	*/
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s.GPIO_OType = GPIO_OType_PP;
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_s.GPIO_Pin = GPIO_Pin_13;
	
	GPIO_Init(GPIOG, &gpio_init_s);
}

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
	initMotor();
	initSweepTIM();
	InitProximitySensor();
	
	
	//example_1a_thread = osThreadCreate(osThread(DisplayRSSIThread), NULL);
	tid_sweep = osThreadCreate(osThread(sweep_thread), NULL);
	masterStationThreadId = osThreadCreate(osThread(ROLES_MasterStationThread), &tid_sweep);
	
	osKernelStart ();                         // start thread execution 
}


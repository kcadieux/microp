#include <stdio.h>
#include "ProximitySensor.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

uint16_t data;


int tick = 0;


const int PERIOD = 420;
const int PRESCALAR = 10000;
const int MAIN_TIMER_FREQ = 84000000 / (PERIOD * PRESCALAR);
const int BLINK_FREQ = MAIN_TIMER_FREQ / 2;

void TIM3_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	tick = 1;
}

static void InitMainTimer()
{
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
	TIM_TimeBaseStructure.TIM_Prescaler = PRESCALAR - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	// TIM IT enable
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	// TIM3 enable counter
	TIM_Cmd(TIM3, ENABLE);
}

int main(){
	InitProximitySensor();	
	RequestData();
	InitMainTimer();
	
		if (tick) {
		while(1){
		if (IsDataReady()){
		data= GetFilteredData();
			printf("\n %d",data);
			}
		}
		}
	return 0;
}


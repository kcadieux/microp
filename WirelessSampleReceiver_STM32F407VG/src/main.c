#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "wireless.h"
#include "cc2500.h"

static const uint32_t PRESCALER = 10000;
static const uint32_t PERIOD = 8400;

static int ticks = 0;


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

void ContinuousWirelessReception()
{
	uint8_t packet[WLESS_PACKET_SIZE];
	
	if (WLESS_ListenForPacket() != WLESS_StatusCode_LISTEN_SUCCESS) {
		printf("Could not start listening for packets\n");
	}
	
	//Sample program that continuously receives packets
	while (1) {
		if (WLESS_PacketEventOccurred()) {
			WLESS_StatusCodeTypeDef status = WLESS_ReceivePacket(packet);
			
			if (status != WLESS_StatusCode_RX_SUCCESS) {
				printf("A packet could not be received for this reason: %d\n", status);
			} else {
				int i=0;
				printf("Packet received:");
				for (; i<WLESS_PACKET_SIZE; ++i) {
					printf("%d, ", packet[i]);
				}
				printf("\n");
				
				if (WLESS_ListenForPacket() != WLESS_StatusCode_LISTEN_SUCCESS) {
					printf("Could not start listening for packets\n");
				}
			}
		}
	}
}

int main(void)
{
	CC2500_InitTypeDef cc2500_init_s;
	CC2500_PATABLETypeDef patable;
	InitMainTimer();
	while (ticks != 5);
	
	//Init Wireless communication module
	WLESS_Init();
	
	CC2500_ReadConfiguration(&cc2500_init_s);
	CC2500_ReadPATABLE(&patable);
	
	ContinuousWirelessReception();
	
	while(1);
	
}


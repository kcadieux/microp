#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "cc2500.h"

static const uint32_t PRESCALER = 10000;
static const uint32_t PERIOD = 9000;

static int ticks = 0;


void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s.GPIO_OType = GPIO_OType_PP;
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_s.GPIO_Pin = GPIO_Pin_13;
	
	GPIO_Init(GPIOG, &gpio_init_s);
}

static uint8_t regs[50];

int main(void)
{
	static int status = 0;
	uint8_t* p = regs;
	
	InitMainTimer();
	
	while (ticks != 5);
	
	CC2500_Init();
	
	//Test writing all configuration registers and reading them back
#define CC2500_CONFIG_REGISTER(name, addr, def_val) CC2500_WriteConfigRegister(addr, def_val);
	CC2500_CONFIG_REGISTERS
#undef CC2500_CONFIG_REGISTER
	
#define CC2500_CONFIG_REGISTER(name, addr, def_val) *p++ =  CC2500_ReadConfigRegister(addr);
	CC2500_CONFIG_REGISTERS
#undef CC2500_CONFIG_REGISTER

	//Test reading all status registers
	p = regs;
#define CC2500_STATUS_REGISTER(name, addr) *p++ =  CC2500_ReadStatusRegister(addr | 0xC0);
	CC2500_STATUS_REGISTERS
#undef CC2500_STATUS_REGISTER
	
	//Test a command strobe
	status = CC2500_SendCommandStrobe(CC2500_SNOP_W);
	
	while (1) {
	}
	
	
}


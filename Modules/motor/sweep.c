#include <stdio.h>
#include "stm32f4xx.h"
#include "motor.h"
#include "sweep.h"
#include "ProximitySensor.h"

#define DETECTION_THRESHOLD 30
#define ANGLE_OFFSET 90
#define MAX_DISTANCE 150

static NVIC_InitTypeDef NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_InitStruct;
static TIM_OCInitTypeDef  TIM_OCInitStructure;

int some_angle = Default_Angle;
int angle_modifier = 1;
int distance = 0;

int zero_vector[180];
int NOT_DONE = 1;

void initSweepTIM() {
	   uint16_t PrescalerValue = 0;
     
    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		//180; 168 //21429
    //Effective prescaler value for clock frequency
    PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 20000) - 1;
     
    uint32_t tim_period = 400;
    //Timer peripheral clock setup 
    TIM_InitStruct.TIM_Prescaler = 0;               
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;            // Count-up timer mode (justa matter of choice)
    TIM_InitStruct.TIM_Period = tim_period - 1;                     // To get a 25 Hz frequency
    //The timer has only one task, thus there should be no clock division.
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;                // Divide clock by 1; e.g. no division
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
 
    //The reload mode immediate indicates that the prescale value is to be updated immediately
    TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
 
    /* Output Compare Timing Mode configuration: Channel1 */
    //Simply use the timer as a timer.
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    //Eable the output state
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //CC1 update rate = TIM3 counter clock / CCR1_Val = ... Hz
    //Where TIM3 counter clock = (SystemCoreClock/2)/PrescalerValue
    //1 tenth of the timer period is more than enough to be detected.
    TIM_OCInitStructure.TIM_Pulse = tim_period/10;          //Used to control output waveform
    //Required for input capture.
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
 
    //We wish the capture be performed at valid transition, so prescaler is disabled
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
 
    //Enable specified TIM interrupt.
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
 
    /* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);
     
    /* Set Timer 2 interrupt request channel*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                         
    //7 segment should have a lower prioriy than the data input (which has priority 1).
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;    
    //Set sub priority, which is less in weight than preemption priority.
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;                    
    //Enable the interrupt channel (IRQ channel)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
}

int compareWithZero(int distance, int angle) {
	int difference = zero_vector[angle+ANGLE_OFFSET] - distance;
	if (difference < 0) {
		difference = 0;
	}
	if (difference > DETECTION_THRESHOLD) {
		printf("Difference: %d\n", difference);
		return 1; //found something
	}
	printf("Difference: %d\n", difference);
	return 0;
}

void zerosweep() {
	int count = 0;
	NOT_DONE = 1;
	while(NOT_DONE == 1) {
		osSignalWait(MOTOR_TICK_SIGNAL, osWaitForever);
		setMotorAngle(some_angle);
		
		if (some_angle >= ANGLE_OFFSET) {
			angle_modifier = -1;
		} else if (some_angle <= -ANGLE_OFFSET) {
			angle_modifier = +1;
		}
		
		//read distance//
		
		while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) continue;
		distance = GetFilteredData();
		distance = 77504/distance;
		
		if (distance >= MAX_DISTANCE) {
			distance = MAX_DISTANCE;
		}
		
		printf("Angle: %d, Distance: %d\n", some_angle, distance);
		zero_vector[some_angle+ANGLE_OFFSET] = distance;
		
		some_angle += angle_modifier;
		count++;
		if (count > 362) {
			NOT_DONE = 0;
			some_angle = 0;
			setMotorAngle(some_angle);
		}
	}
}

void sweep180(position * pos) {
	int somethingIsThere, sumDistance, numDistances, sumAngle, count;
	setMotorAngle(-90);
	osDelay(200);
	for(count = 0; count < 181; count++) 
	{
		osSignalWait(MOTOR_TICK_SIGNAL, osWaitForever);
		setMotorAngle(some_angle);
		
		if (some_angle >= ANGLE_OFFSET) {
			angle_modifier = -1;
		} else if (some_angle <= -ANGLE_OFFSET) {
			angle_modifier = +1;
		}
		
		//read distance//
		
		while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		//if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET) {
				
		distance = GetFilteredData();
		//printf("distance: %d\n", distance);
		distance = 77504/distance;
		//}
		
		if (distance > MAX_DISTANCE) {
			distance = MAX_DISTANCE;
		}
		
		somethingIsThere = compareWithZero(distance, some_angle);
		
		if (somethingIsThere == 1) {
			sumDistance += distance;
			numDistances++;
			sumAngle += some_angle;
		}
		
		//printf("Angle: %d, Distance: %d\n", some_angle, distance);
		
		some_angle += angle_modifier;
		//return 
		//compare with zero values//
	}
	
	pos->angle = sumAngle / numDistances;
	pos->distance = sumDistance / numDistances;
	
}

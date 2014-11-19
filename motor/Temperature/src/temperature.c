#include "temperature.h"

uint_fast32_t AlarmFlag;

static GPIO_InitTypeDef  GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_InitStruct;
static TIM_OCInitTypeDef  TIM_OCInitStructure;
static ADC_InitTypeDef adc_init_s;
static ADC_CommonInitTypeDef adc_common_init_s;
 
static const float GL_MaxAllowableTemp = 30.0;                          //Upper threshold of processor temperature in degree Celcius.
static const int STD_ALARM_TIM_PERIOD = 10000;                          //Maximum CCR value allowable.
static int increment = STD_ALARM_TIM_PERIOD/25;                         //Standard size of CCR value increment/decrement.
 
void tempTimerInit(void)
{
    uint16_t PrescalerValue = 0;
     
    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
    //Effective prescaler value for clock frequency
    PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 20000) - 1;
     
    uint32_t tim_period = 800;
    //Timer peripheral clock setup 
    TIM_InitStruct.TIM_Prescaler = 0;               
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;            // Count-up timer mode (justa matter of choice)
    TIM_InitStruct.TIM_Period = tim_period - 1;                     // To get a 25 Hz frequency for temperature sensor sampling.
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
}//end of temp_timer_init

/// <summary>
/// Converts a given voltage value read from ADC to 
/// the temperature value in Celsius.
/// </summary>
/// <remarks>
/// Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25
/// Where:
/// -V25 = VSENSE value for 25° C (0.76V, looking at the data sheet)
/// -Avg_Slope = average slope of the temperature vs. 
///  VSENSE curve (2.5mV/Celsius degrees, looking at the data sheet)
/// </remarks>
/// <param_name="voltageValue">
/// The voltage value, in mV 
/// </param>
/// <returns>
/// The temperature, in Celsius degrees
/// </returns>
float ConvertToTemperature(float voltageValue)
{	
	voltageValue *= VREF; // VREF = 3V
	voltageValue /= MV_CONVERSION; //Reading in mV
	voltageValue /= VOLT_IN_MV; //Reading in Volts
	voltageValue -= REFERENCE_VOLTAGE_AT_25C; // Subtract the reference voltage at 25°C
	voltageValue /= SLOPE25; // Divide by slope 2.5mV
	voltageValue += TEMPERATURE_OFFSET; // Add the 25°C
	
	return voltageValue;
}

/// <summary>
/// Initializes the ADC temperature sensor
/// </summary>
void initTemperatureSensor()
{
	ADC_InitTypeDef adc_init_s;
	ADC_CommonInitTypeDef adc_common_init_s;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // Set the clock	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //Clock Gating
	
	adc_common_init_s.ADC_Mode = ADC_Mode_Independent;
  adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div2;
  adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adc_common_init_s); //Initialization
	
	adc_init_s.ADC_Resolution = ADC_Resolution_12b;
	adc_init_s.ADC_ScanConvMode = DISABLE;
  adc_init_s.ADC_ContinuousConvMode = DISABLE;
  adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  adc_init_s.ADC_DataAlign = ADC_DataAlign_Right;
  adc_init_s.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc_init_s); //Initialization
	
	ADC_Cmd(ADC1, ENABLE); //Enable Module
	
	ADC_TempSensorVrefintCmd(ENABLE);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC
	
	
	
	tempTimerInit();
}

/// <summary>
/// Reads the temperature from the ADC
/// </summary>
/// <returns>
/// The temperature in Celsius degrees
/// </returns>
float readTemperature()
{
	ADC_SoftwareStartConv(ADC1); //Start the conversion
	//Wait on TIM2 to send interrupt request.
	osSignalWait(0x2, osWaitForever);        
	ADC_ClearFlag (ADC1, ADC_FLAG_EOC);	
	AddValueToWindow(ConvertToTemperature(ADC_GetConversionValue(ADC1)));
	return GetAverageWindow(); //returns the average of the window
}



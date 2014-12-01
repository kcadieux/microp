/// <summary>
/// This file contains everything related to the obtention of a moving
/// window average, in float. 
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include "accelerometer.h"

float roll_value;
float pitch_value;
int buffer[3];
int acc_flag = 0;
float calibratedBuffer[3];
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;
static GPIO_InitTypeDef  GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
osThreadId t_id;

/// <summary>
///	Initializes the NVIC
/// </summary>		
void EXTI_NVIC_init()
{
	/* Enable SYSCFG clock: RCC_APB2Periph_SYSCFG correspond to the peripheral handling extInt */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//Set GPIO Int2 Pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/* Connect EXTI Line1 to PE1 pin;  EXTI_PortSourceGPIOE = Line1 */
	SYSCFG_EXTILineConfig(LIS302DL_SPI_INT2_EXTI_PORT_SOURCE, LIS302DL_SPI_INT2_EXTI_PIN_SOURCE);

	/* Configure EXTI Line1 */
	EXTI_InitStructure.EXTI_Line = LIS302DL_SPI_INT2_EXTI_LINE;					//free ext interrupt line.
	
	//As opposed to generating an event, generating an interrupt is more prompt for response.
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	
	// A choice made: if you want it immediately respond to a trigger, use EXTI_Trigger_Rising.
	// if you want a response at end of trigger, use EXTI_Trigger_Falling.  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	
	//To enable the exterior interrupt line
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line1 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = LIS302DL_SPI_INT2_EXTI_IRQn;							//EXTI Line1 Interrupt 
	
	//Set preemption priority.  First priority list considered.
	//Data sampling in this lab should have highest priority.
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	
	
	//Set sub priority, which is less in weight than preemption priority.
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					
	
	//Enable the interrupt channel (IRQ channel)
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}//end of EXTI_NVIC_init


/// <summary>
///	Initializes the accelerometer
/// </summary>		
void init_accel(osThreadId tid_accel)
{
	t_id = tid_accel;
	/* Set configuration of LIS302DL*/
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	//Set the data/signal rate to 100 Hz
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	//Enable data intake from all three directions
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
	//FullScale in the range of +2g to -2g
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	//No need for self test
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);
	
	//Wait for configuration to finish.
	/* Configure Interrupt control register: enable Click interrupt1 */
  uint8_t ctrl = 0x20;		//0x07 = 111, should be 0x20 = 00|100|000 if you want data ready instead of click events
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);

	
	EXTI_NVIC_init();
	
}

/// <summary>
///	Computes the roll from a given set of coordinates
/// </summary>		
/// <param_name="coords">
/// A set of coordinates (x, y, z)
/// </param>
/// <returns>
/// The computed roll value
/// </returns>
float roll(float* coords)
{
	float tempFloat = 0;
	tempFloat = atan (coords[1]/sqrt(coords[0]*coords[0] + coords[2]*coords[2])) * 180 / PI;
	
	if (coords[2] < 0) {
		if (tempFloat < 0) {
			return -180 - tempFloat;
		} else {
			return 180 - tempFloat;
		}
	} else {
		return tempFloat;
	}
}

/// <summary>
///	Computes the pitch from a given set of coordinates
/// </summary>		
/// <param_name="coords">
/// A set of coordinates (x, y, z)
/// </param>
/// <returns>
/// The computed pitch value
/// </returns>
float pitch(float* coords)
{
	float tempFloat = 0;
	tempFloat = atan (coords[0]/sqrt(coords[1]*coords[1] + coords[2]*coords[2])) * 180 / PI;
	
	if (coords[2] < 0) {
		if (tempFloat < 0) {
			return -180 - tempFloat;
		} else {
			return 180 - tempFloat;
		}
	} else {
		return tempFloat;
	}
}

/// <summary>
/// Handler of the IRQ of EXTI1
/// </summary>
void EXTI1_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line1) != RESET) {
		LIS302DL_ReadACC(buffer);
		EXTI_ClearITPendingBit(EXTI_Line1);
		
    osSignalSet(t_id, 0x2);
	}
}

/// <summary>
/// Calibrates the accelerometer inputs
/// </summary>
/// <param_name="buff">
/// The accelerometer values
/// </param>
/// <param_name="calibrated">
/// The calibrated values to be outputted
/// </param>
void calibrate(int* buff, float* calibrated) {
	calibrated[0] = (int)buff[0]*calibMatrix[0] + (int)buff[0]*calibMatrix[3] + (int)buff[0]*calibMatrix[6] + calibMatrix[9];
	calibrated[1] = (int)buff[1]*calibMatrix[1] + (int)buff[1]*calibMatrix[4] + (int)buff[1]*calibMatrix[7] + calibMatrix[10];
	calibrated[2] = (int)buff[2]*calibMatrix[2] + (int)buff[2]*calibMatrix[5] + (int)buff[2]*calibMatrix[8] + calibMatrix[11];
}

/// <summary>
/// Filters the accelerometer inputs
/// </summary>
/// <param_name="buff">
/// The accelerometer values
/// </param>
void filter(int* buff)
{
	AddValuesToWindows(buff[0], buff[1], buff[2]);
	buff[0] = GetAverageX();
	buff[1] = GetAverageY();
	buff[2] = GetAverageZ();	
}

float getRoll()	
{
	osSignalWait(0x2, osWaitForever);  
	filter(buffer);
	calibrate(buffer, calibratedBuffer);
	roll_value = roll(calibratedBuffer);				
	acc_flag = 0;
	return roll_value;
}
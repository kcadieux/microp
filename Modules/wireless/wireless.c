#include <string.h>
#include <stdio.h>
#include "wireless.h"
#include "cc2500.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

static const uint8_t ACTUAL_PACKET_SIZE_TX = WLESS_PACKET_SIZE + 1;
static const uint8_t ACTUAL_PACKET_SIZE_RX = ACTUAL_PACKET_SIZE_TX + 2;

static int packet_count = 0;
static int packet_event = 0;
static uint8_t rssi = 0;

static void WaitForPacketEvent(void);
static void InitInterrupt(void);

void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line7);
	packet_event = 1;
	packet_count++;
}

void WLESS_Init(WLESS_InitTypeDef* init_s)
{
	CC2500_InitTypeDef cc2500_init_s;
	CC2500_PATABLETypeDef patable;
	
	CC2500_StructInit(&cc2500_init_s);
	memset(patable.data, 0, CC2500_PATABLE_SIZE);
	patable.data[0] = 0xFE; //Full power transmission
	
	cc2500_init_s.FSCTRL1 = 0x0C;
	cc2500_init_s.FSCTRL0 = 0x00;
	
	//Frequency configuration
	cc2500_init_s.FREQ2 = 0x5D;
	cc2500_init_s.FREQ1 = 0x93;
	cc2500_init_s.FREQ0 = 0xB1;
	
	//Modem configuration
	cc2500_init_s.MDMCFG4 = CC2500_MDMCFG4_CHANBW_E(0) | CC2500_MDMCFG4_CHANBW_M(0) | CC2500_MDMCFG4_DRATE_E(14);
	cc2500_init_s.MDMCFG3 = CC2500_MDMCFG3_DRATE_M(59);
	cc2500_init_s.MDMCFG2 = CC2500_MDMCFG2_ModFormat_MSK | CC2500_MDMCFG2_SyncMode_30_32;
	cc2500_init_s.MDMCFG1 = CC2500_MDMCFG1_NumPreamble_8 | CC2500_MDMCFG1_CHANSPC_E(2);
	cc2500_init_s.MDMCFG0 = CC2500_MDMCFG0_CHANSPC_M(248);
	
	cc2500_init_s.DEVIATN = CC2500_DEVIATN_E(0) | CC2500_DEVIATN_M(0);
	
	//Front end configuration
	cc2500_init_s.FREND1 = CC2500_FREND1_LNA_CURRENT(2) | CC2500_FREND1_LNA2MIX_CURRENT(3) | CC2500_FREND1_LODIV_BUF_CURRENT_RX(1) | CC2500_FREND1_MIX_CURRENT(2);
	cc2500_init_s.FREND0 = CC2500_FREND0_LODIV_BUF_CURRENT_TX(1) | CC2500_FREND0_PA_POWER(0);
	
	//Main radio control state machine configuration 
	cc2500_init_s.MCSM1 = CC2500_MCMS1_CcaMode_RSSI_NO_PACKET | CC2500_MCMS1_RxOffMode_IDLE | CC2500_MCMS1_TxOffMode_IDLE;
	cc2500_init_s.MCSM0 = CC2500_MCMS0_FsAutocal_IDLE_TO_RXTX | CC2500_MCMS0_PoTimeout_64;
  
	//Frequency offset compensation configuration
	cc2500_init_s.FOCCFG = CC2500_FOCCFG_FocPreK_4 | CC2500_FOCCFG_FocPostK_DIV2 | CC2500_FOCCFG_FocLimit_BW_DIV8;
	
	//Bit synchronization configuration
	cc2500_init_s.BSCFG = CC2500_BSCFG_BsPreKI_1 | CC2500_BSCFG_BsPreKP_2 | CC2500_BSCFG_BsPostKI_DIV2 | CC2500_BSCFG_BsPostKP_1 | CC2500_BSCFG_BsLimit_0;
	
	//AGC control configuration
	cc2500_init_s.AGCTRL2 = CC2500_AGCCTRL2_MaxDvgaGain_3_HIGHEST | CC2500_AGCCTRL2_MaxLnaGain_MAX | CC2500_AGCCTRL2_MagnTarget_42;
	cc2500_init_s.AGCTRL1 = CC2500_AGCCTRL1_AgcLnaPriority_LNA | CC2500_AGCCTRL1_CarrierSenseRelThr_DISABLED | CC2500_AGCCTRL1_CARRIER_SENSE_ABS_THR(0);
	cc2500_init_s.AGCTRL0 = CC2500_AGCCTRL0_HystLevel_MEDIUM | CC2500_AGCCTRL0_WaitTime_32 | CC2500_AGCCTRL0_AgcFreeze_NEVER | CC2500_AGCCTRL0_FilterLength_8_OOK4;
	
	//Frequency synthesizer calibration configuration
	cc2500_init_s.FSCAL3 = 0xEA;
	cc2500_init_s.FSCAL2 = 0x0A;
	cc2500_init_s.FSCAL1 = 0x00;
	cc2500_init_s.FSCAL0 = 0x19;
	
	//FIFO threshold configuration
	cc2500_init_s.FIFOTHR = CC2500_FifoThreshold_TX09_RX56;
	
	//GDO configuration
	cc2500_init_s.IOCFG2 = CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_CHIP_RDYN;
	cc2500_init_s.IOCFG0 = CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_TX_FIFO_THRESHOLD;
	
	//Packet control configuration
	cc2500_init_s.PKTCTRL1 = CC2500_PKTCTRL1_PQT(0) | CC2500_PKTCTRL1_AddrCheck_BROADCAST_0_AND_255  | CC2500_PKTCTRL1_APPEND_STATUS;
	cc2500_init_s.PKTCTRL0 = CC2500_PKTCTRL0_PacketFormat_FIFO | CC2500_PKTCTRL0_CRC_EN | CC2500_PKTCTRL0_Length_FIXED;
	cc2500_init_s.PKTLEN = ACTUAL_PACKET_SIZE_TX;
	
	cc2500_init_s.ADDR = init_s->address;
	cc2500_init_s.CHANNR = 0;

	CC2500_Init();
	CC2500_WriteConfig(&cc2500_init_s);
	CC2500_WritePATABLE(&patable);
	
	InitInterrupt();
	
	CC2500_SendCommandStrobe(CC2500_SFRX_R);
}

WLESS_StatusCodeTypeDef WLESS_SendPacketBurst(uint8_t* packetBytes, uint8_t address, uint8_t burstSize)
{
	int i;
	WLESS_StatusCodeTypeDef status = WLESS_StatusCode_TX_SUCCESS;
	
	for (i=0; i<burstSize && status == WLESS_StatusCode_TX_SUCCESS; ++i) {
		status = WLESS_SendPacket(packetBytes, address);
	}
	
	return status;
}

WLESS_StatusCodeTypeDef WLESS_SendPacket(uint8_t* packetBytes, uint8_t address)
{
	uint8_t txBytes;
	CC2500_StatusTypeDef status;
	
	status = CC2500_ReadStatusRegister(CC2500_TXBYTES, &txBytes);
	
	if (txBytes == 0 && status.state == CC2500_StatusState_IDLE) {
		
		//Switch to TX FIFO threshold reached trigger of packet TX trigger
		CC2500_WriteConfigRegister(CC2500_IOCFG0, CC2500_GDO_Trigger_TX_FIFO_THRESHOLD);
		
		status = CC2500_WriteTxFIFO(&address, 1);
		status = CC2500_WriteTxFIFO(packetBytes, WLESS_PACKET_SIZE);
		status = CC2500_SendCommandStrobe(CC2500_STX_W);
		
		WaitForPacketEvent();
		
		return WLESS_StatusCode_TX_SUCCESS;
	}
	
	return WLESS_StatusCode_CHIP_BUSY_ERROR;
}

WLESS_StatusCodeTypeDef WLESS_ReceivePacketVerified(uint8_t* packetBytes)
{
	WLESS_StatusCodeTypeDef status = WLESS_ReceivePacket(packetBytes);
	
	while (status == WLESS_StatusCode_RX_CRC_ERROR) {
		status = WLESS_ReceivePacket(packetBytes);
	}
	
	return status;
}

WLESS_StatusCodeTypeDef WLESS_ReceivePacket(uint8_t* packetBytes)
{
	uint8_t rxBytes;
	uint8_t appendedStatus[2];
	CC2500_StatusTypeDef status = CC2500_ReadStatusRegister(CC2500_RXBYTES, &rxBytes);
	
	if (status.state != CC2500_StatusState_IDLE) return WLESS_StatusCode_CHIP_BUSY_ERROR;
	
	//Switch to end of packet RX trigger
	CC2500_WriteConfigRegister(CC2500_IOCFG0, CC2500_GDO_Trigger_RX_FIFO_THRESHOLD_END_PACKET);
	
	CC2500_SendCommandStrobe(CC2500_SRX_R);
	WaitForPacketEvent();
	
	status = CC2500_ReadStatusRegister(CC2500_RXBYTES, &rxBytes);
	if (rxBytes < ACTUAL_PACKET_SIZE_RX) return WLESS_StatusCode_RX_FIFO_NO_PACKET_AVAILABLE_ERROR;
	
	CC2500_ReadRxFIFO(packetBytes, 1); //Get rid of the address
	CC2500_ReadRxFIFO(packetBytes, WLESS_PACKET_SIZE); //Grab packet
	CC2500_ReadRxFIFO(appendedStatus, 2); //Grab status
	
	rssi = appendedStatus[0];
	
	if (appendedStatus[1] & 0x80) return WLESS_StatusCode_RX_SUCCESS;
	else return WLESS_StatusCode_RX_CRC_ERROR;
}

uint8_t WLESS_GetLatestRSSI(void)
{
	return rssi;
}

static void WaitForPacketEvent()
{
	while (!packet_event);
	while (CC2500_SendCommandStrobe(CC2500_SNOP_W).state != CC2500_StatusState_IDLE);
}

static void InitInterrupt()
{
	EXTI_InitTypeDef 	 EXTI_init_s;
  NVIC_InitTypeDef 	 NVIC_init_s;
	GPIO_InitTypeDef	 gpio_init_s;
	
	//GPIO pin for capturing interrupts
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	gpio_init_s.GPIO_Pin = GPIO_Pin_7;
  gpio_init_s.GPIO_Mode = GPIO_Mode_IN;
  gpio_init_s.GPIO_OType = GPIO_OType_PP;
  gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_s.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &gpio_init_s);
	
	//EXTI Configuration
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource7);
	
	EXTI_init_s.EXTI_Line = EXTI_Line7;
	EXTI_init_s.EXTI_LineCmd = ENABLE;
	EXTI_init_s.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_init_s.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_init_s);
	
	//NVIC config
	NVIC_init_s.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_init_s.NVIC_IRQChannelCmd = ENABLE;
	NVIC_init_s.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_init_s.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_init_s);
}

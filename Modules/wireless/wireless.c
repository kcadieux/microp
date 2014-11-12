#include <string.h>
#include "wireless.h"
#include "cc2500.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

static int packet_count = 0;
static int packet_event = 0;

#ifndef SMARTRF_CC2500_H
#define SMARTRF_CC2500_H

#define SMARTRF_RADIO_CC2500
#define SMARTRF_SETTING_IOCFG0     0x06
#define SMARTRF_SETTING_PKTCTRL1   0x00
#define SMARTRF_SETTING_PKTCTRL0   0x04
#define SMARTRF_SETTING_FSCTRL1    0x0A
#define SMARTRF_SETTING_FREQ2      0x5D
#define SMARTRF_SETTING_FREQ1      0x93
#define SMARTRF_SETTING_FREQ0      0xB1
#define SMARTRF_SETTING_MDMCFG4    0x2D
#define SMARTRF_SETTING_MDMCFG3    0x3B
#define SMARTRF_SETTING_MDMCFG2    0x73
#define SMARTRF_SETTING_DEVIATN    0x00
#define SMARTRF_SETTING_MCSM0      0x18
#define SMARTRF_SETTING_FOCCFG     0x1D
#define SMARTRF_SETTING_BSCFG      0x1C
#define SMARTRF_SETTING_AGCCTRL2   0xC7
#define SMARTRF_SETTING_AGCCTRL1   0x00
#define SMARTRF_SETTING_AGCCTRL0   0xB0
#define SMARTRF_SETTING_FREND1     0xB6
#define SMARTRF_SETTING_FSCAL3     0xEA
#define SMARTRF_SETTING_FSCAL1     0x00
#define SMARTRF_SETTING_FSCAL0     0x11

#endif

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

void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line7);
	packet_event = 1;
	packet_count++;
}

void WLESS_SmartRF_Init()
{
	CC2500_PATABLETypeDef patable;
	patable.data[0]= 0xFE;
	CC2500_Init();

	CC2500_WriteConfigRegister(CC2500_IOCFG0, SMARTRF_SETTING_IOCFG0);
	CC2500_WriteConfigRegister(CC2500_PKTCTRL1, SMARTRF_SETTING_PKTCTRL1);
	CC2500_WriteConfigRegister(CC2500_PKTCTRL0, SMARTRF_SETTING_PKTCTRL0);
	CC2500_WriteConfigRegister(CC2500_PKTLEN, 0xA);
	CC2500_WriteConfigRegister(CC2500_FSCTRL1, SMARTRF_SETTING_FSCTRL1);
	CC2500_WriteConfigRegister(CC2500_FREQ2, SMARTRF_SETTING_FREQ2);
	CC2500_WriteConfigRegister(CC2500_FREQ1, SMARTRF_SETTING_FREQ1);
	CC2500_WriteConfigRegister(CC2500_FREQ0, SMARTRF_SETTING_FREQ0);
	CC2500_WriteConfigRegister(CC2500_MDMCFG4, SMARTRF_SETTING_MDMCFG4);
	CC2500_WriteConfigRegister(CC2500_MDMCFG3, SMARTRF_SETTING_MDMCFG3);
	CC2500_WriteConfigRegister(CC2500_MDMCFG2, SMARTRF_SETTING_MDMCFG2);
	CC2500_WriteConfigRegister(CC2500_DEVIATN, SMARTRF_SETTING_DEVIATN);
	CC2500_WriteConfigRegister(CC2500_MCSM0, SMARTRF_SETTING_MCSM0);
	CC2500_WriteConfigRegister(CC2500_FOCCFG, SMARTRF_SETTING_FOCCFG);
	CC2500_WriteConfigRegister(CC2500_BSCFG, SMARTRF_SETTING_BSCFG);
	CC2500_WriteConfigRegister(CC2500_AGCTRL2, SMARTRF_SETTING_AGCCTRL2);
	CC2500_WriteConfigRegister(CC2500_AGCTRL1, SMARTRF_SETTING_AGCCTRL1);
	CC2500_WriteConfigRegister(CC2500_AGCTRL0, SMARTRF_SETTING_AGCCTRL0);
	CC2500_WriteConfigRegister(CC2500_FREND1, SMARTRF_SETTING_FREND1);
	CC2500_WriteConfigRegister(CC2500_FSCAL3, SMARTRF_SETTING_FSCAL3);
	CC2500_WriteConfigRegister(CC2500_FSCAL1, SMARTRF_SETTING_FSCAL1);
	CC2500_WriteConfigRegister(CC2500_FSCAL0, SMARTRF_SETTING_FSCAL0);
	
	CC2500_WritePATABLE(&patable);
	
	InitInterrupt();
	
	CC2500_SendCommandStrobe(CC2500_SFRX_R);
}

void WLESS_Init()
{
	CC2500_InitTypeDef cc2500_init_s;
	CC2500_PATABLETypeDef patable;
	
	//WLESS_SmartRF_Init();
	//return;
	
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
	cc2500_init_s.FIFOTHR = CC2500_FifoThreshold_TX33_RX32;
	
	//GDO configuration
	cc2500_init_s.IOCFG2 = CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_CHIP_RDYN;
	cc2500_init_s.IOCFG0 = CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_FIFO_SYNCWORD;
	
	//Packet control configuration
	cc2500_init_s.PKTCTRL1 = CC2500_PKTCTRL1_PQT(0) | CC2500_PKTCTRL1_AddrCheck_NONE;
	cc2500_init_s.PKTCTRL0 = CC2500_PKTCTRL0_PacketFormat_FIFO | CC2500_PKTCTRL0_CRC_EN | CC2500_PKTCTRL0_Length_FIXED;
	cc2500_init_s.PKTLEN = 10;
	
	cc2500_init_s.ADDR = 1;
	cc2500_init_s.CHANNR = 1;

	CC2500_Init();
	CC2500_WriteConfig(&cc2500_init_s);
	CC2500_WritePATABLE(&patable);
	
	InitInterrupt();
	
	CC2500_SendCommandStrobe(CC2500_SFRX_R);
}

WLESS_StatusCodeTypeDef WLESS_SendPacket(uint8_t* packetBytes)
{
	uint8_t txBytes;
	CC2500_StatusTypeDef status;
	
	status = CC2500_ReadStatusRegister(CC2500_TXBYTES, &txBytes);
	
	if (txBytes == 0 && status.state == CC2500_StatusState_IDLE) {
		CC2500_SendCommandStrobe(CC2500_SFSTXON_R);
		CC2500_WriteTxFIFO(packetBytes, WLESS_PACKET_SIZE);
		CC2500_SendCommandStrobe(CC2500_STX_W);
		return WLESS_StatusCode_TX_SUCCESS;
	}
	
	return WLESS_StatusCode_CHIP_BUSY_ERROR;
}

WLESS_StatusCodeTypeDef WLESS_ListenForPacket()
{
	uint8_t rxBytes;
	CC2500_StatusTypeDef status = CC2500_ReadStatusRegister(CC2500_RXBYTES, &rxBytes);
	
	//If there exists a packet in the RX FIFO, the user must read it before listening for another packet
	if (rxBytes > 0) return WLESS_statusCode_RX_FIFO_NOT_EMPTY_ERROR;
	if (status.state != CC2500_StatusState_IDLE) return WLESS_StatusCode_CHIP_BUSY_ERROR;
	
	CC2500_SendCommandStrobe(CC2500_SRX_R);
	return WLESS_StatusCode_LISTEN_SUCCESS;
}

WLESS_StatusCodeTypeDef WLESS_ReceivePacket(uint8_t* packetBytes)
{
	uint8_t rxBytes;
	CC2500_StatusTypeDef status = CC2500_ReadStatusRegister(CC2500_RXBYTES, &rxBytes);
	
	if (rxBytes < WLESS_PACKET_SIZE) return WLESS_StatusCode_RX_FIFO_NO_PACKET_AVAILABLE_ERROR;
	if (status.state != CC2500_StatusState_IDLE) return WLESS_StatusCode_CHIP_BUSY_ERROR;
	
	CC2500_ReadRxFIFO(packetBytes, WLESS_PACKET_SIZE);
	return WLESS_StatusCode_RX_SUCCESS;
}

int WLESS_PacketEventOccurred()
{
	if (packet_event) {
		uint8_t rxBytes;
		CC2500_StatusTypeDef status = CC2500_ReadStatusRegister(CC2500_RXBYTES, &rxBytes);
		
		//Trigger the packet event only when the chip is finished receiving or transmitting
		if (status.state == CC2500_StatusState_IDLE) {
			packet_event = 0;
			return 1;
		}
	}
	
	return 0;
}


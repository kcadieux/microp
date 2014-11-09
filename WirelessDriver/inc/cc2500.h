#ifndef __CC2500_H__
#define __CC2500_H__

#include <stdint.h>



#define CC2500_RW_READ 	0x80
#define CC2500_RW_WRITE	0x00


//GDO Configuration
typedef enum
{
	CC2500_GDO_DriveStength_LOW = 0x00,
	CC2500_GDO_DriveStength_HIGH = 0x80
} CC2500_GDO_DriveStength_TypeDef;

typedef enum
{
	CC2500_GDO_Polarity_LOW = 0x40,
	CC2500_GDO_Polarity_HIGH = 0x00
} CC2500_GDO_Polarity_TypeDef;

#define CC2500_GDO_TEMP_SENSOR_EN		0x80

typedef enum
{
	CC2500_GDO_Trigger_RX_FIFO_THRESHOLD = 						0x00,
	CC2500_GDO_Trigger_RX_FIFO_THRESHOLD_END_PACKET = 0x01,
	CC2500_GDO_Trigger_TX_FIFO_THRESHOLD = 						0x02,
	CC2500_GDO_Trigger_TX_FIFO_FULL = 								0x03,
	CC2500_GDO_Trigger_RX_FIFO_OVERFLOW = 						0x04,
	CC2500_GDO_Trigger_TX_FIFO_UNDERFLOW = 						0x05,
	CC2500_GDO_Trigger_FIFO_SYNCWORD = 								0x06,
	CC2500_GDO_Trigger_RX_FIFO_CRC_OK = 							0x07,
	CC2500_GDO_Trigger_PREAMBLE_QUALITY_REACHED = 		0x08,
	CC2500_GDO_Trigger_CLEAR_CHANNEL = 								0x09,
	CC2500_GDO_Trigger_PLL_LOCK = 										0x0A,
	CC2500_GDO_Trigger_SCLK = 												0x0B,
	CC2500_GDO_Trigger_SYNC_SDO = 										0x0C,
	CC2500_GDO_Trigger_ASYNC_SDO = 										0x0D,
	CC2500_GDO_Trigger_CARRIER_SENSE = 								0x0E,
	CC2500_GDO_Trigger_CRC_OK = 											0x0F,
	
	CC2500_GDO_Trigger_RX_HARD_DATA1 = 								0x16,
	CC2500_GDO_Trigger_RX_HARD_DATA0 = 								0x17,
	
	CC2500_GDO_Trigger_PA_PD = 												0x1B,
	CC2500_GDO_Trigger_LNA_PD = 											0x1C,
	CC2500_GDO_Trigger_RX_SYMBOL_TICK = 							0x1D,
	
	CC2500_GDO_Trigger_WOR_EVNT0 = 										0x24,
	CC2500_GDO_Trigger_WOR_EVNT1 = 										0x25,
	
	CC2500_GDO_Trigger_CLK32K = 											0x27,
	
	CC2500_GDO_Trigger_CHIP_RDYN = 										0x29,
	
	CC2500_GDO_Trigger_XOSC_STABLE = 									0x2B,
	
	CC2500_GDO_Trigger_GDO0_Z_EN_N = 									0x2D,
	CC2500_GDO_Trigger_TRI_STATE =										0x2E,
	CC2500_GDO_Trigger_HW_TO_0 = 											0x2F,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_1 = 							0x30,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_1_5 = 						0x31,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_2 =								0x32,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_3 =								0x33,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_4 =								0x34,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_6 =								0x35,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_8 =								0x36,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_12 =							0x37,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_16 =							0x38,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_24 =							0x39,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_32 =							0x3A,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_48 =							0x3B,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_64 =							0x3C,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_96 =							0x3D,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_128 =							0x3E,
	CC2500_GDO_Trigger_CLK_XOSC_DIV_192 =							0x3F
} CC2500_GDO_Trigger_TypeDef;

//FIFO Threshold Configuration
typedef enum
{
	CC2500_FifoThreshold_TX61_RX04 = 	0x0,
	CC2500_FifoThreshold_TX57_RX08 = 	0x1,
	CC2500_FifoThreshold_TX53_RX12 = 	0x2,
	CC2500_FifoThreshold_TX49_RX16 = 	0x3,
	CC2500_FifoThreshold_TX45_RX20 = 	0x4,
	CC2500_FifoThreshold_TX41_RX24 = 	0x5,
	CC2500_FifoThreshold_TX37_RX28 = 	0x6,
	CC2500_FifoThreshold_TX33_RX32 = 	0x7,
	CC2500_FifoThreshold_TX29_RX36 = 	0x8,
	CC2500_FifoThreshold_TX25_RX40 = 	0x9,
	CC2500_FifoThreshold_TX21_RX44 = 	0xA,
	CC2500_FifoThreshold_TX17_RX48 = 	0xB,
	CC2500_FifoThreshold_TX13_RX52 = 	0xC,
	CC2500_FifoThreshold_TX09_RX56 = 	0xD,
	CC2500_FifoThreshold_TX05_RX60 = 	0xE,
	CC2500_FifoThreshold_TX01_RX64 = 	0xF,
} CC2500_FifoThreshold_TypeDef;

//Packet Automation Control
#define CC2500_PKTCTRL1_PQT(pqt) 				(pqt << 5)
#define CC2500_PKTCTRL1_CRC_AUTOFLUSH 	0x8
#define CC2500_PKTCTRL1_APPEND_STATUS		0x4

typedef enum
{
	CC2500_PKTCTRL1_AddrCheck_NONE = 								0x00,
	CC2500_PKTCTRL1_AddrCheck_NO_BROADCAST = 				0x01,
	CC2500_PKTCTRL1_AddrCheck_BROADCAST_0 = 				0x2,
	CC2500_PKTCTRL1_AddrCheck_BROADCAST_0_AND_255 = 0x3
} CC2500_PKTCTRL1_AddrCheck_TypeDef;

#define CC2500_PKTCTRL0_WHITE_DATA		0x40

typedef enum
{
	CC2500_PKTCTRL0_PacketFormat_FIFO = 				0x0 << 4,
	CC2500_PKTCTRL0_PacketFormat_SYNC_SERIAL = 	0x1 << 4,
	CC2500_PKTCTRL0_PacketFormat_RANDOM_TX = 		0x2 << 4,
	CC2500_PKTCTRL0_PacketFormat_ASYNC_SERIAL = 0x3 << 4
} CC2500_PKTCTRL0_PacketFormat_TypeDef;

#define CC2500_PKTCTRL0_CC2400_EN			0x08
#define CC2500_PKTCTRL0_CRC_EN				0x04

typedef enum
{
	CC2500_PKTCTRL0_Length_FIXED = 			0x0,
	CC2500_PKTCTRL0_Length_VARIABLE = 	0x1,
	CC2500_PKTCTRL0_Length_INFINITE = 	0x2,
} CC2500_PKTCTRL0_Length_TypeDef;


//Modem Configuration
#define CC2500_MDMCFG4_CHANBW_E(chanbw_e)		(chanbw_e << 6)
#define CC2500_MDMCFG4_CHANBW_M(chanbw_m)		(chanbw_m << 4)
#define CC2500_MDMCFG4_DRATE_E(drate_e)			(drate_e)

#define CC2500_MDMCFG3_DRATE_M(drate_m)			(drate_m)

#define CC2500_MDMCFG2_DEM_DCFILT_OFF				0x80

typedef enum
{
	CC2500_MDMCFG2_ModFormat_2FSK = 0x0 << 4,
	CC2500_MDMCFG2_ModFormat_GFSK = 0x1 << 4,
	CC2500_MDMCFG2_ModFormat_OOK =	0x3 << 4,
	CC2500_MDMCFG2_ModFormat_MSK =	0x7 << 4
} CC2500_MDMCFG2_ModFormat_TypeDef;

#define CC2500_MDMCFG2_MANCHESTER_EN				0x08

typedef enum
{
	CC2500_MDMCFG2_SyncMode_NONE = 								0x0,
	CC2500_MDMCFG2_SyncMode_15_16 = 							0x1,
	CC2500_MDMCFG2_SyncMode_16_16 = 							0x2,
	CC2500_MDMCFG2_SyncMode_30_32 = 							0x3,
	CC2500_MDMCFG2_SyncMode_NONE_CARRIER_SENSE = 	0x4,
	CC2500_MDMCFG2_SyncMode_15_16_CARRIER_SENSE = 0x5,
	CC2500_MDMCFG2_SyncMode_16_16_CARRIER_SENSE = 0x6,
	CC2500_MDMCFG2_SyncMode_30_32_CARRIER_SENSE = 0x7,
} CC2500_MDMCFG2_SyncMode_TypeDef;

#define CC2500_MDMCFG1_FEC_EN				0x80

typedef enum
{
	CC2500_MDMCFG1_NumPreamble_2 = 		0x0 << 4,
	CC2500_MDMCFG1_NumPreamble_3 = 		0x1 << 4,
	CC2500_MDMCFG1_NumPreamble_4 = 		0x2 << 4,
	CC2500_MDMCFG1_NumPreamble_6 = 		0x3 << 4,
	CC2500_MDMCFG1_NumPreamble_8 = 		0x4 << 4,
	CC2500_MDMCFG1_NumPreamble_12 =		0x5 << 4,
	CC2500_MDMCFG1_NumPreamble_16 =		0x6 << 4,
	CC2500_MDMCFG1_NumPreamble_24 =		0x7 << 4,
} CC2500_MDMCFG1_NumPreamble_TypeDef;

#define CC2500_MDMCFG1_CHANSPC_E(chanspc_e)		(chanspc_e)

#define CC2500_MDMCFG0_CHANSPC_M(chanspc_m)		(chanspc_m)


//Modem Deviation Configuration
#define CC2500_DEVIATN_E(e)		(e << 4)
#define CC2500_DEVIATN_M(m)		(m)

//Main Radio Control State Machine Configuration
#define CC2500_MCSM2_RX_TIME_RSSI		0x10
#define CC2500_MCSM2_RX_TIME_QUAL		0x08
#define CC2500_MCSM2_RX_TIME(time)	(time)

typedef enum
{
	CC2500_MCMS1_CcaMode_ALWAYS = 				0x0 << 4,
	CC2500_MCMS1_CcaMode_RSSI = 					0x1 << 4,
	CC2500_MCMS1_CcaMode_NO_PACKET = 			0x2 << 4,
	CC2500_MCMS1_CcaMode_RSSI_NO_PACKET = 0x3 << 4
} CC2500_MCMS1_CcaMode_TypeDef;

typedef enum
{
	CC2500_MCMS1_RxOffMode_IDLE = 	0x0 << 2,
	CC2500_MCMS1_RxOffMode_FSTXON = 0x1 << 2,
	CC2500_MCMS1_RxOffMode_TX = 		0x2 << 2,
	CC2500_MCMS1_RxOffMode_RX = 		0x3 << 2
} CC2500_MCMS1_RxOffMode_TypeDef;

typedef enum
{
	CC2500_MCMS1_TxOffMode_IDLE = 	0x0,
	CC2500_MCMS1_TxOffMode_FSTXON = 0x1,
	CC2500_MCMS1_TxOffMode_TX = 		0x2,
	CC2500_MCMS1_TxOffMode_RX = 		0x3
} CC2500_MCMS1_TxOffMode_TypeDef;

typedef enum
{
	CC2500_MCMS0_FsAutocal_NEVER = 								0x0 << 4,
	CC2500_MCMS0_FsAutocal_IDLE_TO_RXTX = 				0x1 << 4,
	CC2500_MCMS0_FsAutocal_RXTX_TO_IDLE = 				0x2 << 4,
	CC2500_MCMS0_FsAutocal_RXTX_TO_IDLE_EVERY_4 = 0x3 << 4
} CC2500_MCMS0_FsAutocal_TypeDef;

typedef enum
{
	CC2500_MCMS0_PoTimeout_1 = 		0x0 << 2,
	CC2500_MCMS0_PoTimeout_16 = 	0x1 << 2,
	CC2500_MCMS0_PoTimeout_64 = 	0x2 << 2,
	CC2500_MCMS0_PoTimeout_256 = 	0x3 << 2
} CC2500_MCMS0_PoTimeout_TypeDef;

#define CC2500_MCMS0_PIN_CTRL_EN		0x2
#define CC2500_MCMS0_XOSC_FORCE_ON	0x1


//Frequency Offset Compensation Configuration
#define CC2500_FOCCFG_FOC_BS_CS_GATE		0x20

typedef enum
{
	CC2500_FOCCFG_FocPreK_1 = 	0x0 << 3,
	CC2500_FOCCFG_FocPreK_2 = 	0x1 << 3,
	CC2500_FOCCFG_FocPreK_3 = 	0x2 << 3,
	CC2500_FOCCFG_FocPreK_4 = 	0x3 << 3
} CC2500_FOCCFG_FocPreK_TypeDef;

typedef enum
{
	CC2500_FOCCFG_FocPostK_SAME = 	0x0 << 2,
	CC2500_FOCCFG_FocPostK_DIV2 = 	0x1 << 2
} CC2500_FOCCFG_FocPostK_TypeDef;

typedef enum
{
	CC2500_FOCCFG_FocLimit_0 = 0x0,
	CC2500_FOCCFG_FocLimit_BW_DIV8 = 0x1,
	CC2500_FOCCFG_FocLimit_BW_DIV4 = 0x2,
	CC2500_FOCCFG_FocLimit_BW_DIV2 = 0x3
} CC2500_FOCCFG_FocLimit_TypeDef;


//Bit Synchronization Configuration
typedef enum
{
	CC2500_BSCFG_BsPreKI_1 = 	0x0 << 6,
	CC2500_BSCFG_BsPreKI_2 = 	0x1 << 6,
	CC2500_BSCFG_BsPreKI_3 = 	0x2 << 6,
	CC2500_BSCFG_BsPreKI_4 = 	0x3 << 6
} CC2500_BSCFG_BsPreKI_TypeDef;

typedef enum
{
	CC2500_BSCFG_BsPreKP_1 = 	0x0 << 4,
	CC2500_BSCFG_BsPreKP_2 = 	0x1 << 4,
	CC2500_BSCFG_BsPreKP_3 = 	0x2 << 4,
	CC2500_BSCFG_BsPreKP_4 = 	0x3 << 4
} CC2500_BSCFG_BsPreKP_TypeDef;

typedef enum
{
	CC2500_BSCFG_BsPostKI_SAME = 	0x0 << 3,
	CC2500_BSCFG_BsPostKI_DIV2 = 	0x1 << 3
} CC2500_BSCFG_BsPostKI_TypeDef;

typedef enum
{
	CC2500_BSCFG_BsPostKP_SAME = 	0x0 << 2,
	CC2500_BSCFG_BsPostKP_1 = 		0x1 << 2
} CC2500_BSCFG_BsPostKP_TypeDef;

typedef enum
{
	CC2500_BSCFG_BsLimit_0 = 			0x0,
	CC2500_BSCFG_BsLimit_3_125 = 	0x1,
	CC2500_BSCFG_BsLimit_6_25 = 	0x2,
	CC2500_BSCFG_BsLimit_12_5 = 	0x3
} CC2500_BSCFG_BsLimit_TypeDef;

//AGC Control Configuration
typedef enum
{
	CC2500_AGCCTRL2_MaxDvgaGain_ALL = 				0x0 << 6,
	CC2500_AGCCTRL2_MaxDvgaGain_HIGHEST = 		0x1 << 6,
	CC2500_AGCCTRL2_MaxDvgaGain_2_HIGHEST = 	0x2 << 6,
	CC2500_AGCCTRL2_MaxDvgaGain_3_HIGHEST = 	0x3 << 6
} CC2500_AGCCTRL2_MaxDvgaGain_TypeDef;

typedef enum
{
	CC2500_AGCCTRL2_MaxLnaGain_MAX = 					0x0 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_2_6_BELOW = 		0x1 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_6_1_BELOW = 		0x2 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_7_4_BELOW = 		0x3 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_9_2_BELOW = 		0x4 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_11_5_BELOW = 	0x5 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_14_6_BELOW = 	0x6 << 3,
	CC2500_AGCCTRL2_MaxLnaGain_17_1_BELOW = 	0x7 << 3
} CC2500_AGCCTRL2_MaxLnaGain_TypeDef;

typedef enum
{
	CC2500_AGCCTRL2_MagnTarget_24 = 	0x0,
	CC2500_AGCCTRL2_MagnTarget_27 = 	0x1,
	CC2500_AGCCTRL2_MagnTarget_30 = 	0x2,
	CC2500_AGCCTRL2_MagnTarget_33 = 	0x3,
	CC2500_AGCCTRL2_MagnTarget_36 = 	0x4,
	CC2500_AGCCTRL2_MagnTarget_38 = 	0x5,
	CC2500_AGCCTRL2_MagnTarget_40 = 	0x6,
	CC2500_AGCCTRL2_MagnTarget_42 = 	0x7,
} CC2500_AGCCTRL2_MagnTarget_TypeDef;

typedef enum
{
	CC2500_AGCCTRL1_AgcLnaPriority_LNA2 = 	0x0 << 6,
	CC2500_AGCCTRL1_AgcLnaPriority_LNA = 		0x1 << 6
} CC2500_AGCCTRL1_AgcLnaPriority_TypeDef;

typedef enum
{
	CC2500_AGCCTRL1_CarrierSenseRelThr_DISABLED = 	0x0 << 4,
	CC2500_AGCCTRL1_CarrierSenseRelThr_6 = 					0x1 << 4,
	CC2500_AGCCTRL1_CarrierSenseRelThr_10 = 				0x2 << 4,
	CC2500_AGCCTRL1_CarrierSenseRelThr_14 = 				0x3 << 4
} CC2500_AGCCTRL1_CarrierSenseRelThr_TypeDef;

#define CC2500_AGCCTRL1_CARRIER_SENSE_ABS_THR_DISABLED 0x8
#define CC2500_AGCCTRL1_CARRIER_SENSE_ABS_THR(th)	(th)

typedef enum
{
	CC2500_AGCCTRL0_HystLevel_NONE = 		0x0 << 6,
	CC2500_AGCCTRL0_HystLevel_LOW = 		0x1 << 6,
	CC2500_AGCCTRL0_HystLevel_MEDIUM = 	0x2 << 6,
	CC2500_AGCCTRL0_HystLevel_LARGE = 	0x3 << 6
} CC2500_AGCCTRL0_HystLevel_TypeDef;

typedef enum
{
	CC2500_AGCCTRL0_WaitTime_8 = 		0x0 << 4,
	CC2500_AGCCTRL0_WaitTime_16 = 	0x1 << 4,
	CC2500_AGCCTRL0_WaitTime_24 = 	0x2 << 4,
	CC2500_AGCCTRL0_WaitTime_32 = 	0x3 << 4
} CC2500_AGCCTRL0_WaitTime_TypeDef;

typedef enum
{
	CC2500_AGCCTRL0_AgcFreeze_NEVER = 								0x0 << 2,
	CC2500_AGCCTRL0_AgcFreeze_FREEZE_WHEN_SYNCWORD = 	0x1 << 2,
	CC2500_AGCCTRL0_AgcFreeze_FREEZE_ANALOG = 				0x2 << 2,
	CC2500_AGCCTRL0_AgcFreeze_FREEZE_BOTH = 					0x3 << 2
} CC2500_AGCCTRL0_AgcFreeze_TypeDef;

typedef enum
{
	CC2500_AGCCTRL0_FilterLength_8_OOK4 = 		0x0,
	CC2500_AGCCTRL0_FilterLength_16_OOK8 = 		0x1,
	CC2500_AGCCTRL0_FilterLength_32_OOK12 = 	0x2,
	CC2500_AGCCTRL0_FilterLength_64_OOK16 = 	0x3
} CC2500_AGCCTRL0_FilterLength_TypeDef;


//Wake on Radio Control
#define CC2500_WORCTRL_RC_POWER_DOWN		0x80

typedef enum
{
	CC2500_WORCTRL_Event1_4 = 	0x0 << 4,
	CC2500_WORCTRL_Event1_6 = 	0x1 << 4,
	CC2500_WORCTRL_Event1_8 = 	0x2 << 4,
	CC2500_WORCTRL_Event1_12 = 	0x3 << 4,
	CC2500_WORCTRL_Event1_16 = 	0x4 << 4,
	CC2500_WORCTRL_Event1_24 = 	0x5 << 4,
	CC2500_WORCTRL_Event1_32 = 	0x6 << 4,
	CC2500_WORCTRL_Event1_48 = 	0x7 << 4,
} CC2500_WORCTRL_Event1_TypeDef;

#define CC2500_WORCTRL_RC_CALIBRATION		0x08

typedef enum
{
	CC2500_WORCTRL_Resolution_0 = 	0x0,
	CC2500_WORCTRL_Resolution_5 = 	0x1,
	CC2500_WORCTRL_Resolution_10 = 	0x2,
	CC2500_WORCTRL_Resolution_15 = 	0x3,
	
} CC2500_WORCTRL_Resolution_TypeDef;


//Front End RX Configuration
#define CC2500_FREND1_LNA_CURRENT(val)					(val << 6)
#define CC2500_FREND1_LNA2MIX_CURRENT(val)			(val << 4)
#define CC2500_FREND1_LODIV_BUF_CURRENT_RX(val)	(val << 2)
#define CC2500_FREND1_MIX_CURRENT(val)					(val)

//Front End TX Configuration
#define CC2500_FREND0_LODIV_BUF_CURRENT_TX(val)	(val << 4)
#define CC2500_FREND0_PA_POWER(val)							(val)

//Production Test Configuration
typedef enum
{
	CS2500_PTEST_TempSensor_ON = 0xBF,
	CS2500_PTEST_TempSensor_OFF = 0x7F
} CS2500_PTEST_TempSensor_TypeDef;

//Various Test Settings
#define CS2500_TEST2_IMPROVED_SENSITIVITY		0x81
#define CS2500_TEST1_IMPROVED_SENSITIVITY		0x35
#define CS2500_TEST0_VCO_SEL_CAL_EN					0x2


//Status Register Flags
#define CS2500_LQI_CRC_OK(lqi_register_value)		(lqi_register_value & 0x80)
#define CS2500_LQI_EST(lqi_register_value)			(lqi_register_value & 0x7F)

typedef enum
{
	CS2500_MARCSTATE_State_SLEEP = 						0x00,
	CS2500_MARCSTATE_State_IDLE = 						0x01,
	CS2500_MARCSTATE_State_XOFF = 						0x02,
	CS2500_MARCSTATE_State_VCOON_MC = 				0x03,
	CS2500_MARCSTATE_State_REGON_MC = 				0x04,
	CS2500_MARCSTATE_State_MANCAL = 					0x05,
	CS2500_MARCSTATE_State_VCOON = 						0x06,
	CS2500_MARCSTATE_State_REGON = 						0x07,
	CS2500_MARCSTATE_State_STARTCAL = 				0x08,
	CS2500_MARCSTATE_State_BWBOOST = 					0x09,
	CS2500_MARCSTATE_State_FS_LOCK = 					0x0A,
	CS2500_MARCSTATE_State_IFADCON = 					0x0B,
	CS2500_MARCSTATE_State_ENDCAL = 					0x0C,
	CS2500_MARCSTATE_State_RX = 							0x0D,
	CS2500_MARCSTATE_State_RX_END = 					0x0E,
	CS2500_MARCSTATE_State_RX_RST = 					0x0F,
	CS2500_MARCSTATE_State_TXRX_SWITCH = 			0x10,
	CS2500_MARCSTATE_State_RXFIFO_OVERFLOW = 	0x11,
	CS2500_MARCSTATE_State_FSTXON = 					0x12,
	CS2500_MARCSTATE_State_TX = 							0x13,
	CS2500_MARCSTATE_State_TX_END = 					0x14,
	CS2500_MARCSTATE_State_RXTX_SWITCH = 			0x15,
	CS2500_MARCSTATE_State_TXFIFO_UNDERFLOW = 0x16,
} CS2500_MARCSTATE_State_TypeDef;

#define CS2500_MARCSTATE_STATE(marcstate_register_value)	((CS2500_MARCSTATE_State_TypeDef)marcstate_register_value & 0x1F)

#define CS2500_PKTSTATUS_CRC_OK(pktstatus_register_value)				(pktstatus_register_value & 0x80)
#define CS2500_PKTSTATUS_CS(pktstatus_register_value)						(pktstatus_register_value & 0x40)
#define CS2500_PKTSTATUS_PQT_REACHED(pktstatus_register_value)	(pktstatus_register_value & 0x20)
#define CS2500_PKTSTATUS_CCA(pktstatus_register_value)					(pktstatus_register_value & 0x10)
#define CS2500_PKTSTATUS_SFD(pktstatus_register_value)					(pktstatus_register_value & 0x08)
#define CS2500_PKTSTATUS_GDO2(pktstatus_register_value)					(pktstatus_register_value & 0x04)
#define CS2500_PKTSTATUS_GDO0(pktstatus_register_value)					(pktstatus_register_value & 0x01)

#define CS2500_TXBYTES_TXFIFO_UNDERFLOW(txbytes_register_value)	(txbytes_register_value & 0x80)
#define CS2500_TXBYTES_NUM_TXBYTES(txbytes_register_value)			(txbytes_register_value & 0x7F)

#define CS2500_RXBYTES_RXFIFO_OVERFLOW(rxbytes_register_value)	(rxbytes_register_value & 0x80)
#define CS2500_RXBYTES_NUM_RXBYTES(rxbytes_register_value)			(rxbytes_register_value & 0x7F)

#define CS2500_RCCTRL1_STATUS(rcctrl1_register_value)						(rcctrl1_register_value & 0x7F)
#define CS2500_RCCTRL0_STATUS(rcctrl0_register_value)						(rcctrl0_register_value & 0x7F)



//                       Name       Addr  	Default value (From Datasheet)          
#define CC2500_CONFIG_REGISTERS \
	CC2500_CONFIG_REGISTER(IOCFG2, 		0x00, 	CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_CHIP_RDYN) \
	CC2500_CONFIG_REGISTER(IOCFG1, 		0x01, 	CC2500_GDO_DriveStength_LOW | CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_TRI_STATE) \
	CC2500_CONFIG_REGISTER(IOCFG0, 		0x02, 	CC2500_GDO_Polarity_HIGH | CC2500_GDO_Trigger_CLK_XOSC_DIV_192) \
	CC2500_CONFIG_REGISTER(FIFOTHR, 	0x03, 	CC2500_FifoThreshold_TX33_RX32) \
	CC2500_CONFIG_REGISTER(SYNC1, 		0x04, 	0xD3) \
	CC2500_CONFIG_REGISTER(SYNC0, 		0x05, 	0x91) \
	CC2500_CONFIG_REGISTER(PKTLEN, 		0x06, 	0xFF) \
	CC2500_CONFIG_REGISTER(PKTCTRL1,	0x07, 	CC2500_PKTCTRL1_PQT(0) | CC2500_PKTCTRL1_APPEND_STATUS | CC2500_PKTCTRL1_AddrCheck_NONE) \
	CC2500_CONFIG_REGISTER(PKTCTRL0, 	0x08, 	CC2500_PKTCTRL0_WHITE_DATA | CC2500_PKTCTRL0_PacketFormat_FIFO | CC2500_PKTCTRL0_CRC_EN | CC2500_PKTCTRL0_Length_VARIABLE) \
	CC2500_CONFIG_REGISTER(ADDR, 			0x09, 	0x0) \
	CC2500_CONFIG_REGISTER(CHANNR, 		0x0A, 	0x0) \
	CC2500_CONFIG_REGISTER(FSCTRL1, 	0x0B, 	0xF) \
	CC2500_CONFIG_REGISTER(FSCTRL0, 	0x0C, 	0x0) \
	CC2500_CONFIG_REGISTER(FREQ2, 		0x0D, 	0x5E) \
	CC2500_CONFIG_REGISTER(FREQ1, 		0x0E, 	0xC4) \
	CC2500_CONFIG_REGISTER(FREQ0, 		0x0F, 	0xEC) \
	CC2500_CONFIG_REGISTER(MDMCFG4, 	0x10, 	CC2500_MDMCFG4_CHANBW_E(2) | CC2500_MDMCFG4_CHANBW_M(0) | CC2500_MDMCFG4_DRATE_E(12)) \
	CC2500_CONFIG_REGISTER(MDMCFG3, 	0x11, 	CC2500_MDMCFG3_DRATE_M(34)) \
	CC2500_CONFIG_REGISTER(MDMCFG2, 	0x12, 	CC2500_MDMCFG2_ModFormat_2FSK | CC2500_MDMCFG2_SyncMode_16_16) \
	CC2500_CONFIG_REGISTER(MDMCFG1, 	0x13, 	CC2500_MDMCFG1_NumPreamble_4 | CC2500_MDMCFG1_CHANSPC_E(2)) \
	CC2500_CONFIG_REGISTER(MDMCFG0, 	0x14, 	CC2500_MDMCFG0_CHANSPC_M(248)) \
	CC2500_CONFIG_REGISTER(DEVIATN, 	0x15, 	CC2500_DEVIATN_E(4) | CC2500_DEVIATN_M(7)) \
	CC2500_CONFIG_REGISTER(MCSM2, 		0x16, 	CC2500_MCSM2_RX_TIME(7)) \
	CC2500_CONFIG_REGISTER(MCSM1, 		0x17, 	CC2500_MCMS1_CcaMode_RSSI_NO_PACKET | CC2500_MCMS1_RxOffMode_IDLE | CC2500_MCMS1_TxOffMode_IDLE) \
	CC2500_CONFIG_REGISTER(MCSM0, 		0x18, 	CC2500_MCMS0_FsAutocal_NEVER | CC2500_MCMS0_PoTimeout_16) \
	CC2500_CONFIG_REGISTER(FOCCFG, 		0x19, 	CC2500_FOCCFG_FOC_BS_CS_GATE | CC2500_FOCCFG_FocPreK_3 | CC2500_FOCCFG_FocPostK_DIV2 | CC2500_FOCCFG_FocLimit_BW_DIV4) \
	CC2500_CONFIG_REGISTER(BSCFG, 		0x1A, 	CC2500_BSCFG_BsPreKI_2 | CC2500_BSCFG_BsPreKP_3 | CC2500_BSCFG_BsPostKI_DIV2 | CC2500_BSCFG_BsPostKP_1 | CC2500_BSCFG_BsLimit_0) \
	CC2500_CONFIG_REGISTER(AGCTRL2, 	0x1B, 	CC2500_AGCCTRL2_MaxDvgaGain_ALL | CC2500_AGCCTRL2_MaxLnaGain_MAX | CC2500_AGCCTRL2_MagnTarget_33) \
	CC2500_CONFIG_REGISTER(AGCTRL1, 	0x1C, 	CC2500_AGCCTRL1_AgcLnaPriority_LNA | CC2500_AGCCTRL1_CarrierSenseRelThr_DISABLED | CC2500_AGCCTRL1_CARRIER_SENSE_ABS_THR(0)) \
	CC2500_CONFIG_REGISTER(AGCTRL0, 	0x1D, 	CC2500_AGCCTRL0_HystLevel_MEDIUM | CC2500_AGCCTRL0_WaitTime_16 | CC2500_AGCCTRL0_AgcFreeze_NEVER | CC2500_AGCCTRL0_FilterLength_16_OOK8) \
	CC2500_CONFIG_REGISTER(WOREVT1, 	0x1E, 	0x87) \
	CC2500_CONFIG_REGISTER(WOREVT0, 	0x1F, 	0x6B) \
	CC2500_CONFIG_REGISTER(WORCTRL, 	0x20, 	CC2500_WORCTRL_RC_POWER_DOWN | CC2500_WORCTRL_Event1_48 | CC2500_WORCTRL_RC_CALIBRATION | CC2500_WORCTRL_Resolution_0) \
	CC2500_CONFIG_REGISTER(FREND1, 		0x21, 	CC2500_FREND1_LNA_CURRENT(1) | CC2500_FREND1_LNA2MIX_CURRENT(1) | CC2500_FREND1_LODIV_BUF_CURRENT_RX(1) | CC2500_FREND1_MIX_CURRENT(2)) \
	CC2500_CONFIG_REGISTER(FREND0, 		0x22, 	CC2500_FREND0_LODIV_BUF_CURRENT_TX(1) | CC2500_FREND0_PA_POWER(0)) \
	CC2500_CONFIG_REGISTER(FSCAL3, 		0x23, 	0xA9) \
	CC2500_CONFIG_REGISTER(FSCAL2, 		0x24, 	0x0A) \
	CC2500_CONFIG_REGISTER(FSCAL1, 		0x25, 	0x20) \
	CC2500_CONFIG_REGISTER(FSCAL0, 		0x26, 	0x0D) \
	CC2500_CONFIG_REGISTER(RCCTRL1, 	0x27, 	0x41) \
	CC2500_CONFIG_REGISTER(RCCTRL0, 	0x28, 	0x0) \
	CC2500_CONFIG_REGISTER(PTEST, 		0x2A, 	CS2500_PTEST_TempSensor_OFF) \
	CC2500_CONFIG_REGISTER(TEST2, 		0x2C, 	0x88) \
	CC2500_CONFIG_REGISTER(TEST1, 		0x2D, 	0x31) \
	CC2500_CONFIG_REGISTER(TEST0, 		0x2E, 	0x0B)
//end of CC2500_CONFIG_REGISTERS

//                       Name,            Addr
#define CC2500_STATUS_REGISTERS \
	CC2500_STATUS_REGISTER(PARTNUM,					0x30) \
	CC2500_STATUS_REGISTER(VERSION,					0x31) \
	CC2500_STATUS_REGISTER(FREQEST,					0x32) \
	CC2500_STATUS_REGISTER(LQI,							0x33) \
	CC2500_STATUS_REGISTER(RSSI,						0x34) \
	CC2500_STATUS_REGISTER(MARCSTATE,				0x35) \
	CC2500_STATUS_REGISTER(WORTIME1,				0x36) \
	CC2500_STATUS_REGISTER(WORTIME0,				0x37) \
	CC2500_STATUS_REGISTER(VCO_VC_DAC,			0x39) \
	CC2500_STATUS_REGISTER(TXBYTES,					0x3A) \
	CC2500_STATUS_REGISTER(RXBYTES,					0x3B) \
	CC2500_STATUS_REGISTER(RCCTRL1_STATUS,	0x3C) \
	CC2500_STATUS_REGISTER(RCCTRL0_STATUS,	0x3D)
//end of CC2500_STATUS_REGISTERS

//                      Name,     Addr
#define CC2500_COMMAND_STROBES \
	CC2500_COMMAND_STROBE(SRES,			0x30) \
	CC2500_COMMAND_STROBE(SFSTXON,	0x31) \
	CC2500_COMMAND_STROBE(SXOFF,		0x32) \
	CC2500_COMMAND_STROBE(SCAL,			0x33) \
	CC2500_COMMAND_STROBE(SRX,			0x34) \
	CC2500_COMMAND_STROBE(STX,			0x35) \
	CC2500_COMMAND_STROBE(SIDLE,		0x36) \
	CC2500_COMMAND_STROBE(SWOR,			0x38) \
	CC2500_COMMAND_STROBE(SPWD,			0x39) \
	CC2500_COMMAND_STROBE(SFRX,			0x3A) \
	CC2500_COMMAND_STROBE(SFTX,			0x3B) \
	CC2500_COMMAND_STROBE(SWORRST,	0x3C) \
	CC2500_COMMAND_STROBE(SNOP,			0x3D)
// end of CC2500_COMMAND_STROBES


//Code expansion for configuration registers

//Expand the addresses
#define CC2500_CONFIG_REGISTER(name, addr, def_val) CC2500_##name = addr,
typedef enum
{
	CC2500_CONFIG_REGISTERS
} CC2500_ConfigRegisterAddr_TypeDef;
#undef CC2500_CONFIG_REGISTER

//Expand the init structure
#define CC2500_CONFIG_REGISTER(name, addr, def_val) uint8_t name;
typedef struct
{
	CC2500_CONFIG_REGISTERS
} CC2500_InitTypeDef;
#undef CC2500_CONFIG_REGISTER

//Status register code expansion
//Expand status register addresses (or'ed with 0xC0 because R/W and Burst bits must be 1 for reading status registers)
#define CC2500_STATUS_REGISTER(name, addr) CC2500_##name = addr | 0xC0,
typedef enum
{
	CC2500_STATUS_REGISTERS
} CC2500_StatusRegisterAddr_TypeDef;
#undef CC2500_STATUS_REGISTER

//Command strobe code expansion
//Expand command strobe addresses (2 addresses per probe because we have a "read" version and a "write" version)
#define CC2500_COMMAND_STROBE(name, addr) CC2500_##name##_W = addr, CC2500_##name##_R = addr | CC2500_RW_READ,
typedef enum
{
	CC2500_COMMAND_STROBES
} CC2500_CommandStrobeAddr_TypeDef;
#undef CC2500_COMMAND_STROBE


void		CC2500_Init(void);
void		CC2500_StructInit(CC2500_InitTypeDef* init_s);
uint8_t CC2500_WriteConfigRegister(uint8_t addr, uint8_t byte);
uint8_t CC2500_ReadConfigRegister(uint8_t addr);
uint8_t CC2500_ReadStatusRegister(uint8_t addr);
uint8_t CC2500_SendCommandStrobe(uint8_t strobe);

#endif

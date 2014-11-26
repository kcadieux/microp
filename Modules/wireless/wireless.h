#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include <stdint.h>

#ifdef WLESS_RTX
#define osObjectsPublic
#include "osObjects.h"
#endif

#define WLESS_PACKET_SIZE 2

typedef enum
{
	WLESS_StatusCode_TX_SUCCESS,
	WLESS_StatusCode_TX_BURST_TOO_LARGE_ERROR,
	WLESS_StatusCode_RX_SUCCESS,
	WLESS_StatusCode_RX_CRC_ERROR,
	WLESS_statusCode_RX_FIFO_NOT_EMPTY_ERROR,
	WLESS_StatusCode_RX_FIFO_NO_PACKET_AVAILABLE_ERROR,
	WLESS_StatusCode_CHIP_BUSY_ERROR
} WLESS_StatusCodeTypeDef;


void WLESS_Init(void);

WLESS_StatusCodeTypeDef WLESS_SendPacket(uint8_t* packetBytes, uint8_t address);
WLESS_StatusCodeTypeDef WLESS_SendPacketBurst(uint8_t* packetBytes, uint8_t address, uint8_t burstSize);
WLESS_StatusCodeTypeDef WLESS_ReceivePacket(uint8_t address, uint8_t* packetBytes);
WLESS_StatusCodeTypeDef WLESS_ReceivePacketVerified(uint8_t address, uint8_t* packetBytes);

int8_t WLESS_GetLatestRSSI(void);
int8_t WLESS_GetLatestDecibelRSSI(void);


#endif

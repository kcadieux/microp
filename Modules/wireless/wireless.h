#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include <stdint.h>

#define WLESS_PACKET_SIZE 8

typedef struct
{
	uint8_t		address;
	uint32_t 	carrierFrequency;
} WLESS_InitTypeDef;

typedef enum
{
	WLESS_StatusCode_TX_SUCCESS,
	WLESS_StatusCode_RX_SUCCESS,
	WLESS_StatusCode_RX_CRC_ERROR,
	WLESS_statusCode_RX_FIFO_NOT_EMPTY_ERROR,
	WLESS_StatusCode_RX_FIFO_NO_PACKET_AVAILABLE_ERROR,
	WLESS_StatusCode_CHIP_BUSY_ERROR
} WLESS_StatusCodeTypeDef;

void WLESS_Init(WLESS_InitTypeDef* init_s);

WLESS_StatusCodeTypeDef WLESS_SendPacket(uint8_t* packetBytes, uint8_t address);
WLESS_StatusCodeTypeDef WLESS_SendPacketBurst(uint8_t* packetBytes, uint8_t address, uint8_t burstSize);
WLESS_StatusCodeTypeDef WLESS_ReceivePacket(uint8_t* packetBytes);
WLESS_StatusCodeTypeDef WLESS_ReceivePacketVerified(uint8_t* packetBytes);

uint8_t WLESS_GetLatestRSSI(void);



#endif

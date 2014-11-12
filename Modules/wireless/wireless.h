#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include <stdint.h>

#define WLESS_PACKET_SIZE 10

typedef enum
{
	WLESS_StatusCode_TX_SUCCESS,
	WLESS_StatusCode_RX_SUCCESS,
	WLESS_StatusCode_RX_SUCCESS_MORE_DATA,
	WLESS_StatusCode_LISTEN_SUCCESS,
	WLESS_statusCode_RX_FIFO_NOT_EMPTY_ERROR,
	WLESS_StatusCode_RX_FIFO_NO_PACKET_AVAILABLE_ERROR,
	WLESS_StatusCode_CHIP_BUSY_ERROR
} WLESS_StatusCodeTypeDef;

void WLESS_Init(void);
WLESS_StatusCodeTypeDef WLESS_SendPacket(uint8_t* packetBytes);
WLESS_StatusCodeTypeDef WLESS_ListenForPacket(void);
WLESS_StatusCodeTypeDef WLESS_ReceivePacket(uint8_t* packetBytes);

int WLESS_PacketEventOccurred(void);



#endif

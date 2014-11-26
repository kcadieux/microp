#include <cmsis_os.h>
#include "roles.h"
#include "wireless.h"

void ROLES_SlaveStationThread(const void* stationNumberPtr)
{
	uint8_t stationNumber = *((uint8_t*)stationNumberPtr);
	uint8_t packetIn[WLESS_PACKET_SIZE];
	uint8_t packetOut[WLESS_PACKET_SIZE] = {ROLES_PacketType_STATION_RSSI, 0x00};
	
	while (1)
	{
		if (WLESS_ReceivePacketVerified(ROLES_Address_BADGE_BROADCAST_RECEIVER, packetIn) != WLESS_StatusCode_RX_SUCCESS) continue;
		
		if (packetIn[0] != ROLES_PacketType_BADGE_BROADCAST) continue;
		
		packetOut[1] = WLESS_GetLatestDecibelRSSI();
		
		WLESS_SendPacketBurst(packetOut, ROLES_Address_SLAVE_RSSI_RECEIVER_BASE_ADDR + stationNumber, 10);
	}
}

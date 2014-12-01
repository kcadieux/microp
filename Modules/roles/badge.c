#include "roles.h"
#include "wireless.h"

void ROLES_BadgeThread(const void* argument)
{
	uint8_t packet[WLESS_PACKET_SIZE] = {ROLES_PacketType_DETECTOR_BROADCAST, 0x00};
	
	while (1)
	{
		WLESS_SendPacket(packet, ROLES_Address_DETECTOR_BROADCAST_RECEIVER);
	}
}

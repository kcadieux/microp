#include <cmsis_os.h>
#include "roles.h"
#include "wireless.h"

void ROLES_BadgeThread(const void* argument)
{
	uint8_t packet[WLESS_PACKET_SIZE] = {ROLES_PacketType_BADGE_BROADCAST, 0x00};
	
	while (1)
	{
		WLESS_SendPacketBurst(packet, ROLES_Address_BADGE_BROADCAST_RECEIVER, 10);
		osDelay(50);
	}
}

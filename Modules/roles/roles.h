#ifndef __ROLES_H__
#define __ROLES_H__

typedef enum
{
	ROLES_PacketType_BADGE_BROADCAST,
	ROLES_PacketType_STATION_RSSI
} ROLES_PacketTypeTypeDef; 

typedef enum
{
	ROLES_Address_SLAVE_1_RSSI_RECEIVER = 1,
	ROLES_Address_SLAVE_2_RSSI_RECEIVER,
	ROLES_Address_SLAVE_3_RSSI_RECEIVER,
	ROLES_Address_SLAVE_4_RSSI_RECEIVER,
	ROLES_Address_SLAVE_5_RSSI_RECEIVER,
	ROLES_Address_BADGE_BROADCAST_RECEIVER,
	
} ROLES_AddressTypeDef;

void ROLES_BadgeThread(const void* argument);
void ROLES_SlaveStationThread(const void* argument);
void ROLES_MasterStationThread(const void* argument);

#endif

#ifndef __ROLES_H__
#define __ROLES_H__

typedef enum
{
	ROLES_PacketType_DETECTOR_BROADCAST,
	ROLES_PacketType_PERSON_DETECTED
} ROLES_PacketTypeTypeDef; 

typedef enum
{
	ROLES_Address_DETECTOR_BROADCAST_RECEIVER,
	ROLES_Address_PERSON_DETECTED_RECEIVER,
	
} ROLES_AddressTypeDef;

void ROLES_BadgeThread(const void* argument);
void ROLES_SlaveStationThread(const void* argument);
void ROLES_MasterStationThread(const void* argument);

#endif

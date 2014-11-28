#include "roles.h"
#include "wireless.h"
#include "mvavg.h"
#include <stdio.h>

static const int TRIP_DIFFERENCE_THRESHOLD = 8;
static const int UNTRIP_DIFFERENCE_THRESHOLD = 3;
static int isTripped = 0;
static int isPersonIn = 0;
static int counter = 0;

MVAVG_DEFINE(baselineWindow, 200);
MVAVG_DEFINE(sampleWindow, 10);

static unsigned int ticks = 0;



int abs(int num)
{
	if (num < 0)
	{
		return -1*num;
	}
	
	return num;
}

void detectPerson()
{
	if (isPersonIn)
	{
		isPersonIn = 0;
	}
	else
	{
		isPersonIn = 1;
	}
}


void ROLES_SlaveStationThread(const void* argument)
{
	uint8_t packetIn[WLESS_PACKET_SIZE];
	//uint8_t packetOut[WLESS_PACKET_SIZE] = {ROLES_PacketType_STATION_RSSI, 0x00};
	signed char rssi_db;
	
	MVAVG_Init(&baselineWindow);
	MVAVG_Init(&sampleWindow);
	
	while (1)
	{
		if (WLESS_ReceivePacketVerified(ROLES_Address_BADGE_BROADCAST_RECEIVER, packetIn) != WLESS_StatusCode_RX_SUCCESS) continue;
		if (packetIn[0] != ROLES_PacketType_BADGE_BROADCAST) continue;
		
		rssi_db = WLESS_GetLatestDecibelRSSI();
		
		if (!isTripped) {
			MVAVG_AddValue(&baselineWindow, rssi_db);
		}
		MVAVG_AddValue(&sampleWindow, rssi_db);
		
		if (baselineWindow.isFull && !isTripped && (abs(baselineWindow.average - sampleWindow.average)) >= TRIP_DIFFERENCE_THRESHOLD)
		{
			isTripped = 1;
			detectPerson();
			counter++;
			printf("Tripped %d\n", counter);
		}
		else if (isTripped && (abs(baselineWindow.average - sampleWindow.average)) <= UNTRIP_DIFFERENCE_THRESHOLD / 2)
		{
			isTripped = 0;
		}
		
		//packetOut[1] = WLESS_GetLatestDecibelRSSI();
		
		//WLESS_SendPacketBurst(packetOut, ROLES_Address_SLAVE_RSSI_RECEIVER_BASE_ADDR + stationNumber, 10);
	}
}

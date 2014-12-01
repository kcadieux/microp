#include "roles.h"
#include "wireless.h"
#include "mvavg.h"
#include <stdio.h>

static const int TRIP_DIFFERENCE_THRESHOLD = 7;
static const int UNTRIP_DIFFERENCE_THRESHOLD = 3;
static int isTripped = 0;

MVAVG_DEFINE(baselineWindow, 200);
MVAVG_DEFINE(sampleWindow, 20);

int abs(int num)
{
	if (num < 0)
	{
		return -1*num;
	}
	
	return num;
}


void ROLES_SlaveStationThread(const void* argument)
{
	uint8_t packetIn[WLESS_PACKET_SIZE];
	uint8_t packetOut[WLESS_PACKET_SIZE] = {ROLES_PacketType_PERSON_DETECTED, 0x00};
	uint8_t counter = 0;
	signed char rssi_db;
	
	MVAVG_Init(&baselineWindow);
	MVAVG_Init(&sampleWindow);
	
	while (1)
	{
		if (WLESS_ReceivePacketVerified(ROLES_Address_DETECTOR_BROADCAST_RECEIVER, packetIn) != WLESS_StatusCode_RX_SUCCESS) continue;
		if (packetIn[0] != ROLES_PacketType_DETECTOR_BROADCAST) continue;
		
		rssi_db = WLESS_GetLatestDecibelRSSI();
		
		if (!isTripped) {
			MVAVG_AddValue(&baselineWindow, rssi_db);
		}
		MVAVG_AddValue(&sampleWindow, rssi_db);
		
		if (baselineWindow.isFull && !isTripped && (baselineWindow.average - sampleWindow.average) >= TRIP_DIFFERENCE_THRESHOLD)
		{
			isTripped = 1;
			counter++;
			printf("Tripped   %d, %f, %f\n", counter, baselineWindow.average, sampleWindow.average);
		}
		else if (isTripped && (abs(baselineWindow.average - sampleWindow.average)) <= UNTRIP_DIFFERENCE_THRESHOLD / 2)
		{
			isTripped = 0;
			printf("Untripped %d, %f, %f\n", counter, baselineWindow.average, sampleWindow.average);
			packetOut[1] = counter;
			WLESS_SendPacketBurst(packetOut, ROLES_Address_PERSON_DETECTED_RECEIVER, 10);
		}
	}
}

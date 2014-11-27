#include <cmsis_os.h>
#include "roles.h"
#include "wireless.h"
#include "movingAverageFloat.h"

static const int DETECTION_DIFFERENCE_THRESHOLD = 10;
static int isTripped = 0;
static int isPersonIn = 0;
static int counter = 0;

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


void ROLES_SlaveStationThread(const void* stationNumberPtr)
{
	uint8_t stationNumber = *((uint8_t*)stationNumberPtr);
	uint8_t packetIn[WLESS_PACKET_SIZE];
	uint8_t packetOut[WLESS_PACKET_SIZE] = {ROLES_PacketType_STATION_RSSI, 0x00};
	char rssi_db;
	while (1)
	{
		if (WLESS_ReceivePacketVerified(ROLES_Address_BADGE_BROADCAST_RECEIVER, packetIn) != WLESS_StatusCode_RX_SUCCESS) continue;
		if (packetIn[0] != ROLES_PacketType_BADGE_BROADCAST) continue;
		
		rssi_db = WLESS_GetLatestDecibelRSSI();
		AddValueToWindowBaseline(rssi_db);
		AddValueToWindowSample(rssi_db);
		
		if (IsBaselineBufferFull() && !isTripped && (abs(GetAverageWindowBaseline() - GetAverageWindowSample())) >= DETECTION_DIFFERENCE_THRESHOLD)
		{
			isTripped = 1;
			detectPerson();
			counter++;
			printf("Tripped %d\n", counter);
		}
		else
		{
			isTripped = 0;
		}
		
		packetOut[1] = WLESS_GetLatestDecibelRSSI();
		
		WLESS_SendPacketBurst(packetOut, ROLES_Address_SLAVE_RSSI_RECEIVER_BASE_ADDR + stationNumber, 10);
		osDelay(10);
	}
}

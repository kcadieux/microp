#include <cmsis_os.h>
#include "roles.h"
#include "wireless.h"
#include "sweep.h"

void ROLES_MasterStationThread(const void* sweepThreadId)
{
	//signed char rssi_db;
	uint8_t packet[WLESS_PACKET_SIZE];
	int lastCounter = -1;
	//osSignalSet(*((osThreadId*)sweepThreadId), SWEEP_START_SIGNAL);
	
	while (1)
	{
		if (WLESS_ReceivePacketVerified(ROLES_Address_PERSON_DETECTED_RECEIVER, packet) != WLESS_StatusCode_RX_SUCCESS) continue;
		if (packet[0] != ROLES_PacketType_PERSON_DETECTED) continue;
		
		//Protection against receiving multiple packets that are part of the same burst
		if (packet[1] == lastCounter) continue;
		lastCounter = packet[1];
		
		//A person has been detected. Do something here.
		ToggleSweepIsActive();
		osSignalSet(*((osThreadId*)sweepThreadId), SWEEP_START_SIGNAL);
	}
}

#include "mvavg.h"


void MVAVG_Init(MVAVG_WindowTypeDef* window)
{
	int i;
	window->average = 0;
	window->currentIndex = 0;
	window->isFull = 0;
	for (i=0; i<window->BUFFER_SIZE; ++i) {
		window->buffer[i] = 0.f;
	}
}

void MVAVG_AddValue(MVAVG_WindowTypeDef* window, float value)
{
	unsigned int idx = window->currentIndex;
	window->average -= window->buffer[idx];
	window->buffer[idx] = value / (float)window->BUFFER_SIZE;
	window->average += window->buffer[idx];
	
	idx = (idx + 1) % window->BUFFER_SIZE;
	if (idx == 0) window->isFull = 1;
	window->currentIndex = idx;
}




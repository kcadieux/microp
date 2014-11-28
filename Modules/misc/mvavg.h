#ifndef __MVAVG_H__
#define __MVAVG_H__

typedef struct
{
	const unsigned int BUFFER_SIZE;
	float*const buffer;
	unsigned int currentIndex;
	float average;
	int isFull;
} MVAVG_WindowTypeDef;

void MVAVG_Init(MVAVG_WindowTypeDef* window);
void MVAVG_AddValue(MVAVG_WindowTypeDef* window, float value);

#define MVAVG_DEFINE(name, bufferSize)	\
static float MVAVG_##name##_buffer[bufferSize]; \
static MVAVG_WindowTypeDef name = { .BUFFER_SIZE = bufferSize, .buffer = MVAVG_##name##_buffer, .currentIndex = 0, .average = 0, .isFull = 0};

#endif

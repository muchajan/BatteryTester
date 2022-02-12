
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "stdint.h"

uint16_t RingBuffer_FindMin(uint32_t* pBuffer, const uint16_t bufferSize);
uint16_t RingBuffer_FindMax(uint32_t* pBuffer, const uint16_t bufferSize);
uint32_t RingBuffer_ReplaceElementByNeighbourAverage(uint32_t* pBuffer,
    const uint16_t bufferSize, const uint16_t index);
uint32_t RingBuffer_AverageValue(uint32_t* pBuffer, const uint16_t bufferSize);

#endif /* RING_BUFFER_H */
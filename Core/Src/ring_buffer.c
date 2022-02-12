#include "ring_buffer.h"

uint16_t RingBuffer_FindMin(uint32_t* pBuffer, uint16_t bufferSize)
{
    uint32_t minIndex = 0;
    uint32_t min = pBuffer[minIndex];

    for (uint32_t i = 0; i < bufferSize; i++)
    {
        uint32_t sample = pBuffer[i];
        if (sample < min)
        {
            minIndex = i;
            min = sample;
        }
    }
    return minIndex;
}

uint16_t RingBuffer_FindMax(uint32_t* pBuffer, uint16_t bufferSize)
{
    uint32_t maxIndex = 0;
    uint32_t max = pBuffer[maxIndex];

    for (uint32_t i = 0; i < bufferSize; i++)
    {
        uint32_t sample = pBuffer[i];
        if (sample > max)
        {
            maxIndex = i;
            max = sample;
        }
    }
    return maxIndex;
}

uint32_t RingBuffer_ReplaceElementByNeighbourAverage(uint32_t* pBuffer, uint16_t bufferSize,
    uint16_t index)
{
    uint16_t leftNeighbourIndex;
    uint16_t rightNeighbourIndex;

    if (index == 0)
    {
        leftNeighbourIndex = bufferSize - 2;
    }
    else
    {
        leftNeighbourIndex = index - 1;
    }

    if (index == (bufferSize - 1))
    {
        rightNeighbourIndex = 0;
    }
    else
    {
        rightNeighbourIndex = index + 1;
    }

    return pBuffer[index] = (pBuffer[leftNeighbourIndex] + pBuffer[rightNeighbourIndex]) / 2;
}

uint32_t RingBuffer_AverageValue(uint32_t* pBuffer, const uint16_t bufferSize)
{
    uint32_t sum = 0;
    for (uint16_t i = 0; i < bufferSize; i++)
    {
        sum += pBuffer[i];
    }
    return (sum / bufferSize);
}
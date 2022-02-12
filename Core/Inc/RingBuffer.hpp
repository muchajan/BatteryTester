#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "stdint.h"

// template<uint32_t BUFFER_SIZE>
class RingBuffer
{
public:
    RingBuffer():
        m_Size(100), m_Index(0)
    {
    };
    void Add(uint32_t data)
    {
         m_pBuffer[m_Index] = data;
    }

private:
    uint32_t m_Size;
    uint32_t m_Index;
    template<uint32_t BUFFER_SIZE>
    static uint32_t m_pBuffer[BUFFER_SIZE];
};

// constexpr RingBuffer::RingBuffer(const uint32_t bufferSize):
//     m_Size(bufferSize), m_Index(0), m_pBuffer()
// {
// }

#endif /* RING_BUFFER_H */
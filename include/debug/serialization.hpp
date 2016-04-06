#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

namespace mhe {

template <class T>
size_t serialize(uint8_t*& stream, const T& data)
{
    size_t size = sizeof(T);
    memcpy(stream, reinterpret_cast<const void*>(&data), size);
    stream += size;
    return size;
}

template <>
inline size_t serialize(uint8_t*& stream, const uint32_t& data)
{
    stream[0] = (data >> 24) & 0xff;
    stream[1] = (data >> 16) & 0xff;
    stream[2] = (data >> 8) & 0xff;
    stream[3] = data & 0xff;
    stream += 4;
    return 4;
}

template <>
inline size_t serialize(uint8_t*& stream, const int32_t& data)
{
    stream[0] = (data >> 24) & 0xff;
    stream[1] = (data >> 16) & 0xff;
    stream[2] = (data >> 8) & 0xff;
    stream[3] = data & 0xff;
    stream += 4;
    return 4;
}

}

#endif

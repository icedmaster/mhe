#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

namespace mhe {

template <class T>
size_t serialize(uint8_t* stream, const T& data)
{
    size_t size = sizeof(T);
    memcpy(stream, reinterpret_cast<const void*>(&data), size);
    stream += size;
    return size;
}

}

#endif

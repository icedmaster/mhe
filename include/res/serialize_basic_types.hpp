#ifndef __SERIALIZE_BASIC_TYPES_HPP__
#define __SERIALIZE_BASIC_TYPES_HPP__

#include "math/aabb.hpp"
#include "math/quat.hpp"

namespace mhe {
namespace res {

class Serializer;
class Deserializer;

template <class T>
bool serialize(Serializer& serializer, const vector3<T>& value)
{
    serializer.write("x", value.x());
    serializer.write("y", value.y());
    serializer.write("z", value.z());
    return true;
}

template <class T>
bool serialize(Serializer& serializer, const AABB<T>& value)
{
    serializer.write_basic_type("center", value.center);
    serializer.write_basic_type("extents", value.extents);
    return true;
}

template <class T>
bool serialize(Serializer& serializer, const quat<T>& value)
{
    T x, y, z;
    value.euler(x, y, z);
    serializer.write("x", rad_to_deg(x));
    serializer.write("y", rad_to_deg(y));
    serializer.write("z", rad_to_deg(z));
    return true;
}

template <class T>
bool deserialize(Deserializer& serializer, vector3<T>& value)
{
    serializer.read("x", value.x());
    serializer.read("y", value.y());
    serializer.read("z", value.z());
    return true;
}

template <class T>
bool deserialize(Deserializer& serializer, quat<T>& value)
{
    T x, y, z;
    serializer.read("x", x);
    serializer.read("y", y);
    serializer.read("z", z);
    value.set_euler(deg_to_rad(x), deg_to_rad(y), deg_to_rad(z));
    return true;
}

template <class T>
bool deserialize(Deserializer& serializer, AABB<T>& value)
{
    serializer.read_basic_type("center", value.center);
    serializer.read_basic_type("extents", value.extents);
    return true;
}

}}

#endif

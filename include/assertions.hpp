#ifndef __ASSERTIONS_HPP__
#define __ASSERTIONS_HPP__

namespace mhe {

template <class T, class U>
inline T checked_static_cast(const U& value)
{
	assert(dynamic_cast<T>(value) != nullptr);
	return static_cast<T>(value);
}

}

#endif

#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <string>
#include <cstdlib>
#include <stdint.h>

namespace mhe
{
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

template <class T>
inline T random(T min, T max)
{
	return static_cast<T>(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min) + min);
}

template <class T>
class range
{
public:
	range() :
		low_(T()), high_(T())
	{}

	range(const T& low, const T& high) :
		low_(low), high_(high)
	{}

	const T& low() const
	{
		return low_;
	}

	T& rlow()
	{
		return low_;
	}

	const T& high() const
	{
		return high_;
	}

	T rhigh()
	{
		return high_;
	}

	void set(const T& low, const T& high)
	{
		low_ = low;
		high_ = high;
	}

	void set(const T& value)
	{
		low_ = high_ = value;
	}

	void set_low(const T& low)
	{
		low_ = low;
	}

	void set_high(const T& high)
	{
		high_ = high;
	}

	T get_random() const
	{
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (high_ - low_) + low_;
	}

	T mid() const
	{
		return (high_ - low_) / 2;
	}
private:
	T low_, high_;
};

template <class T, class U>
T min(const T& v1, const U& v2)
{
	return v1 < v2 ? v1 : v2;
}

template <class T, class U>
T max(const T& v1, const U& v2)
{
	return v1 > v2 ? v1 : v2;
}

template <class T, class U>
struct pair
{
	T first;
	U second;

	pair() {}
	pair(const T& f, const U& s) : first(f), second(s) {}
};
}

#define UNUSED(x) (void)x;

#define NOT_IMPLEMENTED(x) (void)x;

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

#endif

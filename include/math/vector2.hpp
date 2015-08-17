#ifndef _VECTOR2D_HPP_
#define _VECTOR2D_HPP_

#include <ostream>
#include "core/types_cast.hpp"

namespace mhe {

template <class T>
class vector2
{
public:
	vector2() :
		x_(0), y_(0)
	{
	}

	vector2(T x, T y) :
		x_(x), y_(y)
	{
	}

	template <class U>
	vector2(const vector2<U>& v) :
		x_(static_cast<T>(v.x())), y_(static_cast<T>(v.y()))
	{
	}

	inline void set(T v)
	{
		x_ = y_ = v;
	}

	inline void set(T x, T y)
	{
		x_ = x; y_ = y;
	}

	inline T x() const
	{
		return x_;
	}

	inline T y() const
	{
		return y_;
	}

	void set_x(T x)
	{
		x_ = x;
	}

	void set_y(T y)
	{
		y_ = y;
	}

	T magnitude() const
	{
		return sqrt(x_ * x_ + y_ * y_);
	}

	void normalize()
	{
		T invm = 1.0f / magnitude();
		x_ *= m;
		y_ *= m;
	}

	inline bool operator== (const vector2& v) const
	{
		return ((x_ == v.x_) && (y_ == v.y_)) ? true : false;
	}

	inline bool operator!= (const vector2& v) const
	{
		return ((x_ != v.x_) || (y_ != v.y_)) ? true : false;
	}

	vector2<T>& operator-= (const vector2& v)
	{
		x_ -= v.x_;
		y_ -= v.y_;
		return *this;
	}

	vector2<T>& operator*= (T v)
	{
		x_ *= v;
		y_ *= v;
		return *this;
	}

	template <class Y>
	vector2<T> operator= (const vector2<Y>& v)
	{
		x_ = static_cast<T>(v.x());
		y_ = static_cast<T>(v.y());
		return *this;
	}

	static vector2 zero()
	{
		return vector2();
	}
private:
	T x_, y_;	
};

template <class T, class U>
inline vector2<T> operator- (const vector2<T>& v1, const vector2<U>& v2)
{
	return vector2<T>(v1.x() - v2.x(), v1.y() - v2.y());
}

template <class T, class U>
inline vector2<T> operator+ (const vector2<T>& v1, const vector2<U>& v2)
{
	return vector2<T>(v1.x() + v2.x(), v1.y() + v2.y());
}

template <class T, class U>
inline vector2<T> operator* (const vector2<T>& v1, const vector2<U>& v2)
{
	return vector2<T>(v1.x() * v2.x(), v1.y() * v2.y());
}

template <class T, class U>
inline vector2<T> operator/ (const vector2<T>& v1, const vector2<U>& v2)
{
	return vector2<T>(v1.x() / v2.x(), v1.y() / v2.y());
}

template <class T, class U>
inline bool operator< (const vector2<T>& v1, const vector2<U>& v2)
{
	return ((v1.x() < v2.x()) && (v1.y() < v2.y()));
}

template <class T, class U>
inline bool operator> (const vector2<T>& v1, const vector2<U>& v2)
{
	return ((v1.x() > v2.x()) && (v1.y() > v2.y()));
}

template <class T>
inline std::ostream& operator<< (std::ostream& s, const vector2<T>& v)
{
	return s << '(' << v.x() << ", " << v.y() << ')';
}

typedef vector2<float> v2d;
typedef vector2<float> vec2;

template<>
inline vec2 types_cast(const string& value)
{
	string first, second;
	bool is_first = true;
	for (size_t i = 0, len = value.length(); i < len; ++i)
	{
		if (value[i] == ' ')
		{
			is_first = false;
			continue;
		}
		if (is_first) first += value[i];
		else second += value[i];
	}
	return vec2(types_cast<float>(first), types_cast<float>(second));
}
}

#endif

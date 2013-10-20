#ifndef _VECTOR2D_HPP_
#define _VECTOR2D_HPP_

#include <ostream>

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
		x_(v.x()), y_(v.y())
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

	template <class Y>
	vector2<T> operator= (const vector2<Y>& v)
	{
		x_ = v.x;
		y_ = v.y;
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
}

#endif

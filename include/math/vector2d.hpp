#ifndef _VECTOR2D_HPP_
#define _VECTOR2D_HPP_

namespace mhe {

template <class T>
class vector2
{
private:
	T x_, y_;
public:
	vector2()
	{
		// create NULL vector
		x_ = y_ = 0;
	}

	vector2(T x, T y)
	{
		x_ = x; y_ = y;
	}

	vector2(const vector2& v)
	{
		x_ = v.x_;
		y_ = v.y_;
	}

	~vector2() {} // do nothing

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

	vector2<T> operator- (const vector2& v) const
	{
		vector2 tmp = *this;
		tmp -= v;
		return tmp;
	}

	template <class Y>
	vector2<T> operator= (const vector2<Y>& v)
	{
		x_ = v.x;
		y_ = v.y;
		return *this;
	}
};

typedef vector2<float> v2d;
}

#endif

#ifndef __QUAT_HPP__
#define __QUAT_HPP__

#include <ostream>

namespace mhe {

template <class T>
class quat
{
public:
	quat()
	{
		set_identity();
	}

	quat(const T& x, const T& y, const T& z, const T& w)
	{
		set(x, y, z, w);
	}

	template <class U>
	quat(const quat<U>& other)
	{
		set(other.x(), other.y(), other.z(), other.w());
	}

	quat(float xangle, float yangle, float zangle)
	{
		set_euler(xangle, yangle, zangle);
	}

	T x() const
	{
		return q_[0];
	}

	T y() const
	{
		return q_[1];
	}

	T z() const
	{
		return q_[2];
	}

	T w() const
	{
		return q_[3];
	}

	void set(const T& x, const T& y, const T& z, const T& w)
	{
		q_[0] = x; q_[1] = y; q_[2] = z; q_[3] = w;
	}

	void set_x(const T& x)
	{
		q_[0] = x;
	}

	void set_y(const T& y)
	{
		q_[1] = y;
	}

	void set_z(const T& z)
	{
		q_[2] = z;
	}

	void set_w(const T& w)
	{
		q_[3] = w;
	}

	void set_euler(float xangle, float yangle, float zangle)
	{
		const float cx = ::cos(xangle * 0.5f);
		const float sx = ::sin(xangle * 0.5f); 
		const float cy = ::cos(yangle * 0.5f);
		const float sy = ::sin(yangle * 0.5f);
		const float cz = ::cos(zangle * 0.5f);
		const float sz = ::sin(zangle * 0.5f);
		T nx = cx * cy * sz - sx * sy * cz;
		T ny = cx * sy * cz + sx * cy * sz;
		T nz = sx * cy * cz - cx * sy * sz;
		T nw = cx * cy * cz + sx * sy * sz;
		set(nx, ny, nz, nw);
	}

	void euler(float& xangle, float& yangle, float& zangle) const
	{
		const float x_sqr = q_[0] * q_[0];
		const float y_sqr = q_[1] * q_[1];
		const float z_sqr = q_[2] * q_[2];
		const float w_sqr = q_[3] * q_[3];

		xangle = ::atan( 2 * (w() * z() + y() * x()) / (1 - 2 * (z_sqr + y_sqr)) );
		yangle = ::asin( 2 * (w() * y() - x() * z()) );
		zangle = ::atan( 2 * (w() * x() + z() * y()) / (1 - 2 * (y_sqr + x_sqr)) );
	}

	const T* data() const
	{
		return q_;
	}

	void set_identity()
	{
		q_[0] = 0; q_[1] = 0; q_[2] = 0; q_[3] = 1;
	}

	quat conjugate() const
	{
		return quat(-q_[0], -q_[1], -q_[2], q_[3]);
	}

	float length_sqr() const
	{
		return q_[0] * q_[0] + q_[1] * q_[1] + q_[2] * q_[2] + q_[3] * q_[3];
	}

	float length() const
	{
		return ::sqrt(length_sqr());
	}

	void normalize()
	{
		const float len = length();
		q_[0] /= len; q_[1] /= len; q_[2] /= len; q_[3] /= len;
	}

	quat normalized() const
	{
		const float len = length();
		return quat(q_[0] / len, q_[1] / len, q_[2] / len, q_[3] / len);
	}

	template <class U>
	quat& operator*= (const quat<U>& other)
	{
		T nx = other.w() * x() + w() * other.x() + (y() * other.z() - z() * other.y());
		T ny = other.w() * y() + w() * other.y() + (z() * other.x() - x() * other.z());
		T nz = other.w() * z() + w() * other.z() + (x() * other.y() - y() * other.x());
		T nw = w() * other.w() - (x() * other.x() + y() * other.y() + z() * other.z());
		set(nx, ny, nz, nw);
		return *this;
	}

	template <class U>
	bool operator== (const quat<U>& other) const
	{
		return ( (q_[0] == other.x()) && (q_[1] == other.y()) &&
				 (q_[2] == other.z()) && (q_[3] == other.w()) );
	}

	template <class U>
	bool operator!= (const quat<U>& other) const
	{
		return !(*this == other);
	}

	static quat identity()
	{
		static quat i;
		return i;
	}
private:
	T q_[4];
};

template <class T, class U>
quat<T> operator* (const quat<T>& a, const quat<U>& b)
{
	quat<T> result(a);
	result *= b;
	return result;
}

template <class T>
inline std::ostream& operator<< (std::ostream& s, const quat<T>& q)
{
	float x, y, z;
	q.euler(x, y, z);
	return s << "[(" << q.x() << ", " << q.y() << ", " << q.z() << ", " << q.w() << "), e(" << x << ", " << y << ", " << z << ")]";
}

typedef quat<float> quatf;

}

#endif

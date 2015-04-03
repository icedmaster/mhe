#ifndef __QUAT_HPP__
#define __QUAT_HPP__

#include <ostream>
#include <cmath>

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

	template <class V>
	quat(const V& axis, float angle)
	{
		set_rotation<V>(axis, angle);
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
		float ax = zangle;
		float ay = yangle;
		float az = xangle;

		const float cx = ::cos(ax * 0.5f);
		const float sx = ::sin(ax * 0.5f); 
		const float cy = ::cos(ay * 0.5f);
		const float sy = ::sin(ay * 0.5f);
		const float cz = ::cos(az * 0.5f);
		const float sz = ::sin(az * 0.5f);

		quat<T> qx(sz, 0.0f, 0.0f, cz);
		quat<T> qy(0.0f, sy, 0.0f, cy);
		quat<T> qz(0.0f, 0.0f, sx, cx);

		// TODO: optimize it
		*this = qx * qz * qy;
	}

	void euler(float& xangle, float& yangle, float& zangle) const
	{
		const float x_sqr = q_[0] * q_[0];
		const float y_sqr = q_[1] * q_[1];
		const float z_sqr = q_[2] * q_[2];
		const float w_sqr = q_[3] * q_[3];

		xangle = ::atan2( 2 * (w() * z() + y() * x()), (1 - 2 * (z_sqr + y_sqr)) );
		yangle = ::asin( 2 * (w() * y() - x() * z()) );
		zangle = ::atan2( 2 * (w() * x() + z() * y()), (1 - 2 * (y_sqr + x_sqr)) );
	}

    template <class V>
    void set_rotation(const V& axis, float angle)
    {
        const float s = ::sin(angle * 0.5f);
        const float c = ::cos(angle * 0.5f);
        q_[0] = axis.x() * s;
        q_[1] = axis.y() * s;
        q_[2] = axis.z() * s;
        q_[3] = c;
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
	quat& operator= (const quat<U>& other)
	{
		set(other.x(), other.y(), other.z(), other.w());
		return *this;
	}

	quat& operator= (const quat& other)
	{
		set(other.x(), other.y(), other.z(), other.w());
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

    // | 1-2(y^2)-2(z^2)  2xy-2wz           2xz+2wy         |
    // | 2xy+2wz          1-2(x^2)-2(z^2)   2yz-2wx         |
    // | 2xz-2wy          2yz+2wx           1-2(x^2)-2(y^2) |
    template <class M>
    M to_matrix() const
    {
        T x2 = q_[0] * q_[0];
        T y2 = q_[1] * q_[1];
        T z2 = q_[2] * q_[2];

        M m;
        m.set(1 - 2 * y2 - 2 * z2, 2 * x() * y() + 2 * w() * z(), 2 * x() * z() - 2 * w() * y(), 0,
              2 * x() * y() - 2 * w() * z(), 1 - 2 * x2 - 2 * z2, 2 * y() * z() + 2 * w() * x(), 0,
              2 * x() * z() + 2 * w() * y(), 2 * y() * z() - 2 * w() * x(), 1 - 2 * x2 - 2 * y2, 0,
              0, 0, 0, 1);
        return m;
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

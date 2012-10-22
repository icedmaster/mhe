#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include "mhe_math.hpp"

namespace mhe
{
class Transform
{
public:
	Transform() {}
	Transform(const matrixf& nm) :
	m(nm)
	{}

	void set_transform(const matrixf& nm)
	{
		m = nm;
	}

	const matrixf& transform() const
	{
		return m;
	}

	void apply_transform(const matrixf& nm)
	{
		m = nm * m;
	}

	void translate(float dx, float dy, float dz)
	{
		matrixf tm;
		tm.set_translate(dx, dy, dz);
		m *= tm;
	}

	void translate(const v3d& t)
	{
		translate(t.x(), t.y(), t.z());
	}

	void rotate_x(float ang)
	{
		matrixf rm;
		rm.set_rotate_x(ang);
		m *= rm;
	}

	void rotate_y(float ang)
	{
		matrixf rm;
		rm.set_rotate_y(ang);
		m *= rm;
	}

	void rotate_z(float ang)
	{
		matrixf rm;
		rm.set_rotate_z(ang);
		m *= rm;
	}

	void rotate(const v3d& v, float angle)
	{
		if (v == x_axis)
			rotate_x(angle);
		else if (v == y_axis)
			rotate_y(angle);
		else if (v == z_axis)
			rotate_z(angle);
	}

	void scale(float sx, float sy, float sz)
	{
		matrixf sm;
		sm.set_scale(sx, sy, sz);
		m *= sm;
	}

	void scale(const v3d& s)
	{
		scale(s.x(), s.y(), s.z());
	}

	void identity()
	{
		m.load_identity();
	}
private:
	matrixf m;
};
}

#endif

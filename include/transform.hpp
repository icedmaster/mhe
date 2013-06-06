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
		m(nm),
		dirty_(false)
	{}

	void set_transform(const matrixf& nm)
	{
		m = nm;
		set_dirty();
	}

	const matrixf& transform() const
	{
		return m;
	}

	void apply_transform(const matrixf& nm)
	{
		m *= nm;
		set_dirty();
	}

	void translate(float dx, float dy, float dz)
	{
		matrixf tm;
		tm.set_translate(dx, dy, dz);
		m *= tm;
		set_dirty();
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
		set_dirty();
	}

	void rotate_y(float ang)
	{
		matrixf rm;
		rm.set_rotate_y(ang);
		m *= rm;
		set_dirty();
	}

	void rotate_z(float ang)
	{
		matrixf rm;
		rm.set_rotate_z(ang);
		m *= rm;
		set_dirty();
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
		m = sm * m;
		set_dirty();
	}

	void scale(const v3d& s)
	{
		scale(s.x(), s.y(), s.z());
	}

	void identity()
	{
		m.load_identity();
		set_dirty();
	}
protected:
	bool dirty() const
	{
		return dirty_;
	}

	void clear_dirty_flag()
	{
		dirty_ = false;
	}
private:
	void set_dirty()
	{
		dirty_ = true;
	}

	matrixf m;   
	bool dirty_;
};
}

#endif

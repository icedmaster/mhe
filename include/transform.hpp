#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include "mhe_math.hpp"

namespace mhe
{
class Transform
{
public:
	Transform() :
		scaling_(1, 1, 1),
		dirty_(false)
	{}

	Transform(const matrixf& nm) :
		m(nm),
		scaling_(1, 1, 1),
		dirty_(false)
	{}

	void set_transform(const matrixf& nm)
	{
		m = initial_ * nm;
	}

	const matrixf& transform() const
	{
		return m;
	}

	void apply_transform(const matrixf& nm)
	{
		m *= nm;
	}

	void set_initial(const matrixf& initial)
	{
		initial_ = initial;
		m = initial * m;
	}

	const matrixf& initial() const
	{
		return initial_;
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
		m = rm * m;
	}

	void rotate_y(float ang)
	{
		matrixf rm;
		rm.set_rotate_y(ang);
		m = rm * m;
	}

	void rotate_z(float ang)
	{
		matrixf rm;
		rm.set_rotate_z(ang);
		m = rm * m;
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
		set_transform(matrixf::identity());
	}

	void translate_to(const v3d& position);
	void translate_by(const v3d& position);
	void rotate_to(float angle_x, float angle_y, float angle_z);
	void rotate_to(const vector3<float>& angles)
	{
		rotate_to(angles.x(), angles.y(), angles.z());
	}
	void rotate_by(float angle_x, float angle_y, float angle_z);
	void scale_to(float sx, float sy, float sz);
	void scale_to(const vector3<float>& s)
	{
		scale_to(s.x(), s.y(), s.z());
	}
	void scale_by(float sx, float sy, float sz);

	const vector3<float>& position() const
	{
		return position_;
	}

	const vector3<float>& rotation() const
	{
		return rotation_;
	}

	const vector3<float>& scaling() const
	{
		return scaling_;
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

	void update_transform();

	void set_position(const vector3<float>& position)
	{
		translate_to(position);
	}
private:
	void set_dirty()
	{
		dirty_ = true;
	}

	matrixf m;   
	matrixf initial_;
	vector3<float> position_; 
	vector3<float> rotation_;
	vector3<float> scaling_;
	bool dirty_;
};
}

#endif

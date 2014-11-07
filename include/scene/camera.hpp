#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "core/ref_ptr.hpp"
#include "math/matrix.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "math/frustum.hpp"

namespace mhe {

struct Context;

struct PerspectiveCameraParameters
{
	float fov;
	float znear;
	float zfar;
};

class Camera : public ref_counter
{
public:
	void init(Context& context, const PerspectiveCameraParameters& parameters,
			  const vec3& position, const vec3& direction, const vec3& up);

	const mat4x4& transform() const
	{
		return transform_;
	}

	const mat4x4& view() const
	{
		return transform_;
	}

	const mat4x4& projection() const
	{
		return projection_;
	}
	
	mat4x4 viewprojection() const
	{
        return view() * projection();
	}

	vec3 position() const
	{
		return -transform_.row_vec3(3);
	}

	void translate_by(const vec3& delta)
	{
		position_ += delta;
		update_view();
		//transform_.set_row(3, -(position() + delta));
	}

	void rotate_by(const quatf& rotation)
	{
		//transform_ *= rotation.to_matrix<mat4x4>();
		rotation_ *= rotation;
		update_view();
	}

	void get(mat4x4& v, mat4x4& p, mat4x4& vp) const;

	const frustumf& camera_frustum() const
	{
		return frustum_;
	}
private:
	void update_view();

	frustumf frustum_;
	mat4x4 transform_;
	mat4x4 projection_;
	quatf rotation_;
	vec3 position_;
};

}

#endif

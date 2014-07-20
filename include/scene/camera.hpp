#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "core/ref_ptr.hpp"
#include "render/transform.hpp"
#include "math/matrix.hpp"
#include "math/vector3.hpp"

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
	Camera(Context& context, const PerspectiveCameraParameters& parameters,
		   const vec3& position, const vec3& direction, const vec3& up);

	Transform& transform()
	{
		return transform_;
	}

	const mat4x4& view() const
	{
		return transform_.transform();
	}

	const mat4x4& projection() const
	{
		return projection_;
	}
	
	mat4x4 viewprojection() const
	{
        return view() * projection();
	}

	void get(mat4x4& v, mat4x4& p, mat4x4& vp) const;
private:
	Transform transform_;
	mat4x4 projection_;
};

}

#endif

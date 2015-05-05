#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "core/ref_ptr.hpp"
#include "math/matrix.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "math/frustum.hpp"
#include "render/instances.hpp"

namespace mhe {

struct Context;
struct CameraData;

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
			  const vec3& position, const vec3& rotation);

	const mat4x4& transform() const
	{
		return world_;
	}

	const mat4x4& view() const
	{
		return view_;
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
		return position_;
	}

	quatf rotation() const
	{
		return rotation_;
	}

	vec3 forward() const
	{
		const vec3& world_fwd = world_.forward_vector();
		return vec3(world_fwd.x(), world_fwd.y(), world_fwd.z());
	}

	vec3 right() const
	{
		const vec3& world_right = world_.side_vector();
		return vec3(world_right.x(), world_right.y(), world_right.z());
	}

	void set(const vec3& position, const quatf& rotation);

	void translate_by(const vec3& delta)
	{
		position_ += delta;
		update_view();
	}

	void rotate_by(const quatf& rotation)
	{
		rotation_ *= rotation;
		update_view();
	}

	void get(mat4x4& v, mat4x4& p, mat4x4& vp) const;
	void get(CameraData& camera_data) const;

	const frustumf& camera_frustum() const
	{
		return frustum_;
	}
private:
	void update_view();

	frustumf frustum_;
	mat4x4 world_;
	mat4x4 view_;
	mat4x4 projection_;
	quatf rotation_;
	vec3 position_;
	float znear_;
	float zfar_;
	float near_height_;
	float near_width_;
	float far_height_;
	float far_width_;
	float angle_;
	float aspect_ratio_;
};

struct CameraNode
{
	TransformInstance::IdType transform_id;
	Camera camera;
};

}

#endif

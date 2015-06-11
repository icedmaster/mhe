#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "core/config.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/string.hpp"
#include "core/array.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"
#include "math/aabb.hpp"
#include "draw_call.hpp"

namespace mhe {

struct LightInstance;
struct NodeInstance;

typedef fixed_capacity_vector<DrawCall, max_scene_dips_number> DrawCalls;
typedef fixed_capacity_vector<DrawCallExplicit, max_scene_dips_number> ExplicitDrawCalls;

// It's just a temporary implementation
class SpaceGrid
{
public:
	void set_global_cubemap(const TextureInstance& cubemap)
	{
		global_cubemap_ = cubemap;
	}

	const TextureInstance& global_cubemap() const
	{
		return global_cubemap_;
	}
private:
	TextureInstance global_cubemap_;
};

struct CameraData
{
	mat4x4 world;
	mat4x4 view;
	mat4x4 proj;
	mat4x4 vp;
	mat4x4 inv_vp;
	mat4x4 inv_v;
	vec3 viewpos;
	frustumf camera_frustum;
	float znear;
	float zfar;
	float near_height;
	float near_width;
	float far_height;
	float far_width;
	float angle;
	float aspect_ratio;
    UniformBuffer::IdType percamera_uniform;

	CameraData() : percamera_uniform(UniformBuffer::invalid_id) {}
};

struct FrustumCullingRequest
{
	mat4x4 vp;
};

struct FrustumCullingResult
{
	array<bool, max_scene_nodes_number> visibility; // TODO: use 1 bit as visibility flag
	size_t visible;
};

template <class Req, class Res>
struct RequestData
{
	Req request;
	Res result;
};

typedef RequestData<FrustumCullingRequest, FrustumCullingResult> FrustumCullingRequestData;

enum ViewId
{
	main_view = 0,
	shadowmap_view0 = max_views_number - max_shadowmap_cascades_number
};

class RenderViewRequests
{
public:
	void reset()
	{
		frustum_culling_request_mask_ = 0;
		::memset(reinterpret_cast<void*>(&frustum_culling_requests_[0]), 0, sizeof(frustum_culling_requests_));
	}

	void register_request(ViewId id, const FrustumCullingRequest& request)
	{
		frustum_culling_requests_[id].request = request;
		frustum_culling_request_mask_ |= (1 << id);
	}

	bool is_frustum_culling_request_active(ViewId id) const
	{
		return (frustum_culling_request_mask_ & (1 << static_cast<uint>(id))) != 0;
	}

	FrustumCullingRequestData& frustum_culling_request_data(ViewId id)
	{
		return frustum_culling_requests_[id];
	}
private:
	array<FrustumCullingRequestData, max_views_number> frustum_culling_requests_;
	uint32_t frustum_culling_request_mask_;
};

struct RenderContext
{
    DrawCalls draw_calls;
	ExplicitDrawCalls explicit_draw_calls;

	NodeInstance* nodes;
	size_t nodes_number;

	LightInstance* lights;
	size_t lights_number;

	CameraData main_camera;

	uint32_t tick;
	uint32_t delta;
	float fdelta;

	AABBf aabb;
	SpaceGrid space_grid;

	RenderViewRequests render_view_requests;
};

const size_t max_textures_per_model = 8;

const size_t max_color_textures = 2;

struct ModelContext
{
    UniformBuffer::IdType transform_uniform;

    ModelContext() : transform_uniform(UniformBuffer::invalid_id) {}
};

}

#endif

#include "render/csm_depth_rendering_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"
#include "render/layouts.hpp"
#include "render/light_instance_methods.hpp"

namespace mhe {

bool CSMDepthRenderingMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(StandartGeometryLayout::handle);

	if (!init_default(context, material_system_context))
		return false;

	return true;
}

void CSMDepthRenderingMaterialSystem::close() {}

void CSMDepthRenderingMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	ASSERT(0, "This method should not be called");
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void CSMDepthRenderingMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	LightInstance::IdType light_idx = LightInstance::invalid_id;
	mat4x4 light_view;
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		if (!render_context.lights[i].light.desc().cast_shadows ||
				render_context.lights[i].light.type() != Light::directional)
				continue;
		light_view = get_light_view_matrix(scene_context, render_context.lights[i].id);
	}

	vec4 lightspace_corners[8];
	vec4 points[8] = { vec4(-1.0f, -1.0f, -1.0f, 1.0f),
										 vec4(-1.0f, -1.0f, +1.0f, 1.0f),
										 vec4(+1.0f, -1.0f, +1.0f, 1.0f),
										 vec4(+1.0f, -1.0f, -1.0f, 1.0f),
										 vec4(-1.0f, +1.0f, -1.0f, 1.0f),
										 vec4(-1.0f, +1.0f, +1.0f, 1.0f),
										 vec4(+1.0f, +1.0f, +1.0f, 1.0f),
										 vec4(+1.0f, +1.0f, -1.0f, 1.0f) };
	vec4 center = vec4(render_context.aabb.center, 0.0f);
	vec4 extents = vec4(render_context.aabb.extents, 1.0f);
	for (int i = 0; i < 8; ++i)
	{
		vec4 p = mul(points[i], extents) + center;
		lightspace_corners[i] = p * light_view;
	}

	vec3 viewspace_frustum_points[8];
	const vec3& fwd = vec3::forward();
	const vec3& right = vec3::right();
	const vec3& up = vec3::up();
	vec3 near_center = fwd * render_context.main_camera.znear;
	vec3 far_center = fwd * render_context.main_camera.zfar;
	// near
	viewspace_frustum_points[0] = near_center + up * render_context.main_camera.near_height - right * render_context.main_camera.near_width;
	viewspace_frustum_points[1] = near_center + up * render_context.main_camera.near_height + right * render_context.main_camera.near_width;
	viewspace_frustum_points[2] = near_center - up * render_context.main_camera.near_height - right * render_context.main_camera.near_width;
	viewspace_frustum_points[3] = near_center - up * render_context.main_camera.near_height + right * render_context.main_camera.near_width;
	// far
	viewspace_frustum_points[4] = far_center + up * render_context.main_camera.far_height - right * render_context.main_camera.far_width;
	viewspace_frustum_points[5] = far_center + up * render_context.main_camera.far_height + right * render_context.main_camera.far_width;
	viewspace_frustum_points[6] = far_center - up * render_context.main_camera.far_height - right * render_context.main_camera.far_width;
	viewspace_frustum_points[7] = far_center - up * render_context.main_camera.far_height + right * render_context.main_camera.far_width;

	mat4x4 view_to_lightspace = render_context.main_camera.inv_v * light_view;
	vec4 lightspace_frustum_points[8];
	for (int i = 0; i < 8; ++i)
		lightspace_frustum_points[i] = vec4(viewspace_frustum_points[i], 1.0f) * view_to_lightspace;
}

}

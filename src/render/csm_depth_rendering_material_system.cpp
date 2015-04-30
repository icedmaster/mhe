#include "render/csm_depth_rendering_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"
#include "render/layouts.hpp"
#include "render/light_instance_methods.hpp"
#include "render/uniforms.hpp"

namespace mhe {

bool CSMDepthRenderingMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(StandartGeometryLayout::handle);

	if (!init_default(context, material_system_context))
		return false;

	UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
	UniformBufferDesc uniform_desc;
	uniform_desc.name = "transform";
	uniform_desc.program = &(default_program(context));
	if (!uniform_buffer.init(uniform_desc))
		return false;
	transform_uniform_id_ = uniform_buffer.id();

	RenderState& render_state = create_and_get(context.render_state_pool);
	RenderStateDesc render_state_desc;
	render_state_desc.depth.enabled = true;
	render_state_desc.viewport.viewport.set(0, 0, shadowmap_default_width, shadowmap_default_height);
	if (!render_state.init(render_state_desc)) return false;

	RenderTarget& render_target = create_and_get(context.render_target_pool);
	RenderTargetDesc desc;
	desc.target = rt_readwrite;
	desc.color_targets = 0;
	desc.use_depth = true;
	desc.use_stencil = false;
	desc.depth_datatype = format_float;
	desc.depth_format = format_d24f;
	desc.width = shadowmap_default_width;
	desc.height = shadowmap_default_height;
	if (!render_target.init(context, desc)) return false;

	render_target_id_ = render_target.id();

	render_target.depth_texture(shadowmap_);

	DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
	draw_call_data.render_target = render_target.id();
	draw_call_data.state = render_state.id();
	draw_call_data_id_ = draw_call_data.id;

	clear_command_.set_driver(&context.driver);

	return true;
}

void CSMDepthRenderingMaterialSystem::close() {}

void CSMDepthRenderingMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	ASSERT(0, "This method should not be called");
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void CSMDepthRenderingMaterialSystem::calculate_projection(mat4x4& proj, const vec4* lightspace_aabb, const mat4x4& light_view, const CameraData& camera_data, float znear, float zfar) const
{
	vec3 viewspace_frustum_points[8];
	const vec3& fwd = -vec3::forward();
	const vec3& right = vec3::right();
	const vec3& up = vec3::up();
	vec3 near_center = fwd * znear;
	vec3 far_center = fwd * zfar;
	float t = ::tan(camera_data.angle * 0.5f);
	float near_height = t * znear;
	float near_width = near_height * camera_data.aspect_ratio;
	float far_height = t * zfar;
	float far_width = far_height * camera_data.aspect_ratio;
	// near
	viewspace_frustum_points[0] = near_center + up * near_height - right * near_width;
	viewspace_frustum_points[1] = near_center + up * near_height + right * near_width;
	viewspace_frustum_points[2] = near_center - up * near_height - right * near_width;
	viewspace_frustum_points[3] = near_center - up * near_height + right * near_width;
	// far
	viewspace_frustum_points[4] = far_center + up * far_height - right * far_width;
	viewspace_frustum_points[5] = far_center + up * far_height + right * far_width;
	viewspace_frustum_points[6] = far_center - up * far_height - right * far_width;
	viewspace_frustum_points[7] = far_center - up * far_height + right * far_width;

	mat4x4 view_to_lightspace = camera_data.inv_v * light_view;
	vec4 lightspace_frustum_points[8];
	for (int i = 0; i < 8; ++i)
		lightspace_frustum_points[i] = vec4(viewspace_frustum_points[i], 1.0f) * view_to_lightspace;

	// find LR and TB points for the projection matrix
	vec3 lightspace_min(9999999.0f, 9999999.0f, 9999999.0f);
	vec3 lightspace_max(-9999999.0f, -9999999.0f, -9999999.0f);
	for (int i = 0; i < 8; ++i)
	{
		lightspace_min = min(lightspace_min, lightspace_frustum_points[i].xyz());
		lightspace_max = max(lightspace_max, lightspace_frustum_points[i].xyz());
	}

	vec3 lightspace_aabb_min(9999999.0f, 9999999.0f, 9999999.0f);
	vec3 lightspace_aabb_max(-9999999.0f, -9999999.0f, -9999999.0f);
	for (int i = 0; i < 8; ++i)
	{
		lightspace_aabb_min = min(lightspace_aabb_min, lightspace_aabb[i].xyz());
		lightspace_aabb_max = max(lightspace_aabb_max, lightspace_aabb[i].xyz());
	}

	proj.set_ortho(lightspace_min.x(), lightspace_max.x(), lightspace_min.y(), lightspace_max.y(), -lightspace_aabb_max.z(), -lightspace_aabb_min.z());
}

void CSMDepthRenderingMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	context.materials[id()].clear();
	clear_command_.reset();

	LightInstance::IdType light_idx = LightInstance::invalid_id;
	mat4x4 light_view;
	Light* light = nullptr;
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		if (!render_context.lights[i].light.desc().cast_shadows ||
				render_context.lights[i].light.type() != Light::directional)
				continue;
		light_view = get_light_view_matrix(scene_context, render_context.lights[i].id);
		render_context.lights[i].light.set_shadowmap_texture(shadowmap_);
		light = &render_context.lights[i].light;
	}

	if (light == nullptr)
		return;

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

	mat4x4 proj;
	calculate_projection(proj, lightspace_corners, light_view, render_context.main_camera, render_context.main_camera.znear, render_context.main_camera.zfar);

	TransformSimpleData transform_data;
	transform_data.vp = light_view * proj;
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_id_);
	uniform.update(transform_data);

	light->set_vp(transform_data.vp);

	for (size_t i = 0; i < render_context.nodes_number; ++i)
	{
		if (!render_context.nodes[i].cast_shadow && !render_context.nodes[i].receive_shadow)
			continue;

		for (size_t j = 0; j < render_context.nodes[i].mesh.instance_parts.size(); ++j)
		{
			const MeshPartInstance& part = render_context.nodes[i].mesh.instance_parts[j];
			DrawCall& draw_call = render_context.draw_calls.add();
			draw_call.render_data = render_context.nodes[i].mesh.mesh.parts[j].render_data;
			Material& material = create_and_get(context.materials[id()]);
			material.shader_program = default_program(context).id();
			material.uniforms[0] = transform_uniform_id_;
			material.id = material.id;
			draw_call.material.material_system = id();
			draw_call.material.id = material.id;

			const MaterialInstance& original_material_instance = part.material;
			const Material& original_material = context.materials[original_material_instance.material_system].get(original_material_instance.id);
			material.uniforms[1] = original_material.uniforms[1];

			draw_call.draw_call_data = draw_call_data_id_;

			draw_call.command = &clear_command_;
			draw_call.pass = 0;
		}
	}
}

}

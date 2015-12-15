#include "render/csm_depth_rendering_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"
#include "render/layouts.hpp"
#include "render/light_instance_methods.hpp"
#include "render/uniforms.hpp"
#include "utils/strutils.hpp"

namespace mhe {

CSMDepthRenderingMaterialSystem::CSMDepthRenderingMaterialSystem() :
	profile_command_("shadowmap")
{}

bool CSMDepthRenderingMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(StandartGeometryLayout::handle);

	if (!init_default(context, material_system_context))
		return false;

	size_t cascades_number = material_system_context.options.get<size_t>("cascades");
	size_t rt_height = material_system_context.options.get<size_t>("height");
	size_t rt_width = material_system_context.options.get<size_t>("width");
	const string& percentage_string = material_system_context.options.get<string>("percentage");
	const std::vector<string>& percentage = utils::split(percentage_string, string(","));
	ASSERT(percentage.size() == cascades_number, "The CSM configuration is invalid");
	for (size_t i = 0; i < cascades_number; ++i)
		percentage_.push_back(types_cast<float>(utils::trim_copy(percentage[i])) / 100.0f);

	RenderTarget& render_target = create_and_get(context.render_target_pool);
	RenderTargetDesc desc;
	desc.target = rt_readwrite;
	desc.color_targets = 0;
	desc.use_depth = true;
	desc.use_stencil = false;
	desc.depth_datatype = format_float;
	desc.depth_format = format_d24f;
	desc.width = rt_width * cascades_number;
	desc.height = rt_height;
	desc.depth_anisotropy = 1.0f;
	desc.depth_filter = texture_filter_nearest;
	if (!render_target.init(context, desc)) return false;

	texture_size_.set(desc.width, desc.height);
	cascade_size_.set(rt_width, rt_height);

	render_target_id_ = render_target.id();

	render_target.depth_texture(shadowmap_);

	for (size_t i = 0; i < cascades_number; ++i)
	{
		UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
		UniformBufferDesc uniform_desc;
		uniform_desc.name = "transform";
		uniform_desc.program = &(default_program(context));
		if (!uniform_buffer.init(uniform_desc))
			return false;
		transform_uniform_id_.push_back(uniform_buffer.id());

		RenderState& render_state = create_and_get(context.render_state_pool);
		RenderStateDesc render_state_desc;
		render_state_desc.viewport.viewport.set(i * rt_width, 0, rt_width, rt_height);
		if (!render_state.init(render_state_desc)) return false;

		DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
		draw_call_data.render_target = render_target.id();
		draw_call_data.state = render_state.id();
		draw_call_data_id_.push_back(draw_call_data.id);
	}

	clear_command_.set_driver(&context.driver);
	profile_command_.set_stages(render_stage_begin_priority | render_stage_end_priority);
	list_of_commands_.add_command(&clear_command_);
	list_of_commands_.add_command(&profile_command_);

	cascades_number_ = cascades_number;

	UberShader::Info info = ubershader(context).info("SKINNING");
	UberShader::Index index;
	index.set(info, 1);
	shader_program_with_skinning_ = ubershader(context).get(index);

	return true;
}

void CSMDepthRenderingMaterialSystem::close() {}

void CSMDepthRenderingMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	ASSERT(0, "This method should not be called");
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void CSMDepthRenderingMaterialSystem::start_frame(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	mat4x4 light_view;
	LightInstance* light_instance = nullptr;
	Light* light = nullptr;
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		if (!render_context.lights[i].light.desc().cast_shadows ||
			render_context.lights[i].light.type() != Light::directional ||
			!render_context.lights[i].light.desc().auto_shadow_configuration)
			continue;
		light_view = get_light_view_matrix(scene_context, render_context.lights[i].id);
		shadow_info_.shadowmap = shadowmap_;
		light = &render_context.lights[i].light;
		light_instance = &render_context.lights[i];
	}

	if (light == nullptr)
		return;

	vec4 aabb_points[8];
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
		aabb_points[i] = p;
	}

	const vec3& light_direction = get_light_direction(scene_context, light_instance->id);
	float max_extents = mhe::max(mhe::max(extents.x(), extents.y()), extents.z());
	rayf r(center.xyz() - mhe::mul(light_direction, extents.xyz()), light_direction, max_extents);
	vec3 input = r.origin, output;
	intersects(input, output, r, render_context.aabb);
	vec3 light_position = input;
	light_view.set_row(3, vec3::zero());
	light_view.multTranslate(-light_position);

	mat4x4 proj[max_cascades_number];
	mat4x4 view[max_cascades_number];
	float znear[max_cascades_number];
	float zfar[max_cascades_number];

	shadow_info_.cascades_number = cascades_number_;
	for (size_t i = 0; i < cascades_number_; ++i)
	{
		zfar[i] = render_context.main_camera.zfar * percentage_[i];
		//znear[i] = i == 0 ? render_context.main_camera.znear : zfar[i - 1];
		znear[i] = render_context.main_camera.znear;
		view[i] = light_view;
		calculate_projection(proj[i], view[i], aabb_points, render_context.main_camera, znear[i], zfar[i], light_direction);

		TransformSimpleData transform_data;
		transform_data.vp = view[i] * proj[i];
		UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_id_[i]);
		uniform.update(transform_data);

		shadow_info_.offset[i] = proj[i].row_vec3(3);
		shadow_info_.scale[i] = vec3(proj[i].element(0, 0), proj[i].element(1, 1), proj[i].element(2, 2));
		shadow_info_.znear[i] = znear[i];
		shadow_info_.zfar[i] = zfar[i];

		shadow_info_.lightvp[i] = transform_data.vp;
		shadow_info_.lightview[i] = view[i];

		FrustumCullingRequest request;
		request.vp = transform_data.vp;
		render_context.render_view_requests.register_request(static_cast<ViewId>(shadowmap_view0 + i), request);
	}

	light->set_shadow_info(&shadow_info_);
}

void CSMDepthRenderingMaterialSystem::calculate_projection(mat4x4& proj, mat4x4& view, const vec4* aabb, const CameraData& camera_data, float znear, float zfar,
	const vec3& lightdir) const
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

	vec4 frustum_points_ws[8];
	vec3 frustum_center_ws;
	for (int i = 0; i < 8; ++i)
	{
		frustum_points_ws[i] = vec4(viewspace_frustum_points[i], 1.0f) * camera_data.inv_v;
	}
	
	vec3 near_diag_center_ws = (frustum_points_ws[0].xyz() + frustum_points_ws[3].xyz()) * 0.5f;
	vec3 far_diag_center_ws = (frustum_points_ws[4].xyz() + frustum_points_ws[7].xyz()) * 0.5f;
	frustum_center_ws = (near_diag_center_ws + far_diag_center_ws) * 0.5f;

	//vec3 light_position = frustum_center_ws - lightdir * (frustum_points_ws[7].xyz() - frustum_points_ws[0].xyz()).magnitude() * 0.5f;
	//view.set_row(3, vec3::zero());
	//view.multTranslate(-light_position);

	vec4 lightspace_aabb[8];
	for (int i = 0; i < 8; ++i)
		lightspace_aabb[i] = aabb[i] * view;
	vec3 lightspace_aabb_min(9999999.0f, 9999999.0f, 9999999.0f);
	vec3 lightspace_aabb_max(-9999999.0f, -9999999.0f, -9999999.0f);
	for (int i = 0; i < 8; ++i)
	{
		lightspace_aabb_min = min(lightspace_aabb_min, lightspace_aabb[i].xyz());
		lightspace_aabb_max = max(lightspace_aabb_max, lightspace_aabb[i].xyz());
	}

	mat4x4 aabb_calculation_view = view;

	vec4 lightspace_frustum_points[8];
	for (int i = 0; i < 8; ++i)
		lightspace_frustum_points[i] = frustum_points_ws[i] * aabb_calculation_view;

	// find LR and TB points for the projection matrix
	vec3 lightspace_min(9999999.0f, 9999999.0f, 9999999.0f);
	vec3 lightspace_max(-9999999.0f, -9999999.0f, -9999999.0f);
	for (int i = 0; i < 8; ++i)
	{
		lightspace_min = min(lightspace_min, lightspace_frustum_points[i].xyz());
		lightspace_max = max(lightspace_max, lightspace_frustum_points[i].xyz());
	}

	vec2 inv_cascade_size = 1.0f / cascade_size_;
	vec2 proj_min = lightspace_min.xy();
	vec2 proj_max = lightspace_max.xy();

	vec2 delta = proj_max - proj_min;
	vec2 offset = delta * inv_cascade_size * 0.5f;
	proj_max += offset;
	proj_min -= offset;

	vec2 fit_coeff = delta * inv_cascade_size;

	proj_max = proj_max / fit_coeff;
	proj_max = floor(proj_max);
	proj_max = proj_max * fit_coeff;

	proj_min = proj_min / fit_coeff;
	proj_min = floor(proj_min);
	proj_min = proj_min * fit_coeff;

	proj.set_ortho(proj_min.x(), proj_max.x(),
		proj_min.y(), proj_max.y(),
		-lightspace_aabb_max.z(), -lightspace_aabb_min.z());
}

void CSMDepthRenderingMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	context.materials[id()].clear();
	clear_command_.reset();

	for (size_t pass = 0; pass < cascades_number_; ++pass)
	{
		const bool* visibility = render_context.render_view_requests.frustum_culling_request_data(static_cast<ViewId>(shadowmap_view0 + pass)).result.visibility.data();
		size_t v = 0;
		for (size_t i = 0; i < render_context.nodes_number; ++i)
		{
			if (!render_context.nodes[i].cast_shadow && !render_context.nodes[i].receive_shadow)
				continue;

			for (size_t j = 0; j < render_context.nodes[i].mesh.instance_parts.size(); ++j)
			{
				if (!visibility[v++]) continue;
				const MeshPartInstance& part = render_context.nodes[i].mesh.instance_parts[j];
				DrawCall& draw_call = render_context.draw_calls.add();
				draw_call.render_data = render_context.nodes[i].mesh.mesh.parts[j].render_data;
				Material& material = create_and_get(context.materials[id()]);
				material.shader_program = render_context.nodes[i].mesh.mesh.skeleton.bones.empty() ? default_program(context).id() :
					shader_program_with_skinning_;
				material.uniforms[0] = transform_uniform_id_[pass];
				material.id = material.id;
				draw_call.material.material_system = id();
				draw_call.material.id = material.id;

				const MaterialInstance& original_material_instance = part.material;
				const Material& original_material = context.materials[original_material_instance.material_system].get(original_material_instance.id);
				material.uniforms[1] = original_material.uniforms[1];

				material.texture_buffers[1] = original_material.texture_buffers[1];

				draw_call.draw_call_data = draw_call_data_id_[pass];

				draw_call.command = &list_of_commands_;
				draw_call.pass = pass;
			}
		}
	}
}

}
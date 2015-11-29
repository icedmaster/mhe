#include "render/gbuffer_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/light_instance_methods.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_globals.hpp"

namespace mhe {

GBufferFillMaterialSystem::GBufferFillMaterialSystem() :
	profile_command_("gbuffer_fill"),
	use_baked_lighting_(true),
	use_normalmapping_(global::use_normalmaps.value())
{
	gbuffer_desc_.target = rt_readwrite;
	gbuffer_desc_.width = gbuffer_desc_.height = 0;
	gbuffer_desc_.color_datatype[0] = format_ubyte;
	gbuffer_desc_.color_datatype[1] = format_float;
	gbuffer_desc_.color_datatype[2] = format_float;
	gbuffer_desc_.color_format[0] = format_rgba;
	gbuffer_desc_.color_format[1] = format_rgba16f;
	gbuffer_desc_.color_format[2] = format_rgba16f;
	gbuffer_desc_.color_targets = 3;
	gbuffer_desc_.depth_format = format_d24f;
	gbuffer_desc_.depth_datatype = format_float;
	gbuffer_desc_.use_depth = true;
	gbuffer_desc_.use_stencil = false;
}

bool GBufferFillMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(StandartGeometryLayout::handle);

	clear_command_.set_driver(&context.driver);
	profile_command_.set_stages(render_stage_begin_priority | render_stage_end_priority);
	list_of_commands_.add_command(&clear_command_);
	list_of_commands_.add_command(&profile_command_);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;

	render_target_ = context.render_target_pool.create();
	RenderTarget& render_target = context.render_target_pool.get(render_target_);
	if (!gbuffer_desc_.width) gbuffer_desc_.width = context.window_system.width();
	if (!gbuffer_desc_.height) gbuffer_desc_.height = context.window_system.height();
	return render_target.init(context, gbuffer_desc_);
}

void GBufferFillMaterialSystem::close()
{}

void GBufferFillMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	standart_material_setup(context, scene_context, instance_parts, parts, model_contexts, count);

	UberShader& shader = ubershader(context);
	const UberShader::Info& normalmap_info = shader.info("NORMALMAP");
	const UberShader::Info& skinning_info = shader.info("SKINNING");

	for (size_t i = 0; i < count; ++i)
	{
		DrawCallData& draw_call_data = context.draw_call_data_pool.get(instance_parts[i].draw_call_data);
		draw_call_data.render_target = render_target_;
		Material& material = context.materials[id()].get(instance_parts[i].material.id);
		size_t use_normalmap = material.textures[normal_texture_unit].id != Texture::invalid_id ? 1 : 0;
		if (!global::use_normalmaps.value())
			use_normalmap = 0;
		size_t use_skinning = parts[i].render_data.layout == SkinnedGeometryLayout::handle ? 1 : 0;

		if (use_skinning)
			material.texture_buffers[animation_texture_unit] = model_contexts[i].animation_texture_buffer;

		material.texture_buffers[baked_light_texture_unit] = model_contexts[i].baked_light_texture_buffer;

		UberShader::Index index;
		index.set(normalmap_info, use_normalmap);
		index.set(skinning_info, use_skinning);
		material.shader_program = shader.get(index);

		UniformBufferDesc uniform_buffer_desc;
		uniform_buffer_desc.unit = material_data_unit;
		uniform_buffer_desc.update_type = uniform_buffer_static;
		uniform_buffer_desc.size = sizeof(PhongMaterialData);
		UniformBuffer& uniform = create_and_get(context.uniform_pool);
		if (!uniform.init(uniform_buffer_desc))
		{
			WARN_LOG("Can't initialize material uniform");
		}
		else
		{
			material.uniforms[material_data_unit] = uniform.id();
			MaterialData material_data;
			if (!context.material_manager.get(material_data, parts[i].material_id))
			{
				WARN_LOG("Can't find a material with id:" << parts[i].material_id);
			}
			PhongMaterialData shader_material_data;
			shader_material_data.diffuse = vec4(material_data.render_data.diffuse, 1.0f);
			shader_material_data.specular = vec4(material_data.render_data.specular, material_data.render_data.specular_shininess);
			shader_material_data.params = vec4(material_data.render_data.glossiness, 0.0f, 0.0f, 0.0f);
			uniform.update(shader_material_data);
		}
	}
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	MATERIAL_UPDATE_WITH_COMMAND(context, scene_context, render_context, update, &list_of_commands_);
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context, MeshPartInstance* nodes, MeshPart* parts, size_t count)
{
	UberShader& shader = ubershader(context);
	const UberShader::Info& normalmap_info = shader.info("NORMALMAP");
	const UberShader::Info& skinning_info = shader.info("SKINNING");
	const UberShader::Info& baked_light_info = shader.info("BAKED_LIGHT");
	UberShader::Index index;
	clear_command_.reset();
	for (size_t i = 0; i < count; ++i)
	{
		Material& material = context.materials[id()].get(nodes[i].material.id);
		material.uniforms[perframe_data_unit] = render_context.main_camera.percamera_uniform;
		size_t use_normalmap = material.textures[normal_texture_unit].id != Texture::invalid_id && use_normalmapping_ ? 1 : 0;
		size_t use_skinning = material.texture_buffers[animation_texture_unit] != TextureBuffer::invalid_id ? 1 : 0;
		size_t use_baked_light = use_baked_lighting_ && material.texture_buffers[baked_light_texture_unit] != TextureBuffer::invalid_id ? 1 : 0;
		index.set(normalmap_info, use_normalmap);
		index.set(skinning_info, use_skinning);
		index.set(baked_light_info, use_baked_light);
		material.shader_program = shader.get(index);

#ifdef MHE_UPDATE_MATERIAL
		MaterialData& material_data = context.material_manager.material_data(parts[i].material_id);
		PhongMaterialData shader_material_data;
		shader_material_data.diffuse = vec4(material_data.render_data.diffuse, 1.0f);
		shader_material_data.specular = vec4(material_data.render_data.specular, material_data.render_data.specular_shininess);
		shader_material_data.params = vec4(material_data.render_data.glossiness, 0.0f, 0.0f, 0.0f);

		UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[material_data_unit]);
		uniform.update(shader_material_data);
#endif
	}
}

void GBufferFillMaterialSystem::setup_uniforms(Material& material, Context& /*context*/, SceneContext& /*scene_context*/, const MeshPartInstance& /*part*/, const ModelContext& model_context)
{
	material.uniforms[0] = transform_uniform_;
	material.uniforms[1] = model_context.transform_uniform;
}

void GBufferFillMaterialSystem::output(Context& context, size_t unit, TextureInstance& texture) const
{
	RenderTarget& render_target = context.render_target_pool.get(render_target_);
	if (unit == gbuffer_depth_render_target_index)
		render_target.depth_texture(texture);
	else
		render_target.color_texture(texture, unit);
}

bool GBufferDrawMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	RenderTargetDesc light_buffer_desc;
	light_buffer_desc.target = rt_readwrite;
	light_buffer_desc.color_datatype[0] = format_float;
	light_buffer_desc.color_format[0] = format_rgba16f;
	light_buffer_desc.color_targets = 1;
	light_buffer_desc.use_depth = false;
	light_buffer_desc.use_stencil = false;
	light_buffer_desc.width = context.window_system.width();
	light_buffer_desc.height = context.window_system.height();
	light_buffer_render_target_ = context.render_target_pool.create();
	RenderTarget& light_render_target = context.render_target_pool.get(light_buffer_render_target_);
	if (!light_render_target.init(context, light_buffer_desc))
		return false;

	const TextureInstance* light_texture;
	light_render_target.color_textures(&light_texture);
	light_buffer_texture_ = light_texture[0];

	// TODO: need to get such settings from the shader
	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;

	for (size_t i = 0; i < max_lights_number; ++i)
	{
		light_uniform_[i] = context.uniform_pool.create();
		UniformBuffer& uniform = context.uniform_pool.get(light_uniform_[i]);
		UniformBufferDesc uniform_buffer_desc;
		uniform_buffer_desc.name = "lights";
		uniform_buffer_desc.program = &default_program(context);
		if (!uniform.init(uniform_buffer_desc))
			return false;
	}

	UberShader::Index index;
	const UberShader::Info& info = ubershader(context).info("LIGHT_TYPE");
	index.set(info, Light::directional);
	ShaderProgram::IdType directional_shader_program_id = ubershader(context).get(index);
	ShaderProgram& directional_shader_program = context.shader_pool.get(directional_shader_program_id);
	for (size_t i = 0; i < max_directional_lights_number; ++i)
	{
		directional_light_uniform_[i] = context.uniform_pool.create();
		UniformBuffer& uniform = context.uniform_pool.get(directional_light_uniform_[i]);
		UniformBufferDesc uniform_buffer_desc;
		uniform_buffer_desc.name = "lights";
		uniform_buffer_desc.program = &directional_shader_program;
		if (!uniform.init(uniform_buffer_desc))
			return false;
	}

	clear_command_.set_driver(&context.driver);
	profile_command_.set_stages(render_stage_begin_priority | render_stage_end_priority);
	list_of_commands_.add_command(&clear_command_);
	list_of_commands_.add_command(&profile_command_);

	return init_meshes(context);
}

bool GBufferDrawMaterialSystem::init_meshes(Context &context)
{
	draw_call_data_ = context.draw_call_data_pool.create();
	DrawCallData& draw_call_data = context.draw_call_data_pool.get(draw_call_data_);
	draw_call_data.render_target = light_buffer_render_target_;
	draw_call_data.state = context.render_state_pool.create();
	RenderState& render_state = context.render_state_pool.get(draw_call_data.state);
	RenderStateDesc desc;
	desc.depth.test_enabled = false;
	desc.depth.write_enabled = false;
	desc.blend.enabled = true;
	desc.blend.dstmode = blend_one;
	desc.blend.srcmode = blend_one;

	desc.rasterizer.cull = cull_none;
	if (!render_state.init(desc)) return false;

	if (!init_fullscreen_quad(context))
		return false;
	if (!init_sphere(context))
		return false;
	return init_conus(context);
}

bool GBufferDrawMaterialSystem::init_fullscreen_quad(Context& context)
{
	quad_mesh_.mesh.parts.resize(1);
	quad_mesh_.instance_parts.resize(1);

	if (!utils::create_fullscreen_quad(quad_mesh_, context))
	{
		ERROR_LOG("Can't create fullscreen quad");
		return false;
	}

	quad_mesh_.mesh.parts[0].render_data.layout = FullscreenLayout::handle;
	quad_mesh_.instance_parts[0].draw_call_data = draw_call_data_;
	return true;
}

bool GBufferDrawMaterialSystem::init_sphere(Context& context)
{
	sphere_mesh_.mesh.parts.resize(1);
	sphere_mesh_.instance_parts.resize(1);
	if (!utils::create_sphere(sphere_mesh_, context, 2))
	{
		ERROR_LOG("Can't create a sphere");
		return false;
	}

	sphere_mesh_.instance_parts[0].draw_call_data = draw_call_data_;

	return true;
}

bool GBufferDrawMaterialSystem::init_conus(Context& context)
{
	conus_mesh_.mesh.parts.resize(1);
	conus_mesh_.instance_parts.resize(1);
	if (!utils::create_conus(conus_mesh_, context, 1.0f, 1.0f, 8))
	{
		ERROR_LOG("Can't create a conus");
		return false;
	}

	conus_mesh_.instance_parts[0].draw_call_data = draw_call_data_;

	return true;
}

void GBufferDrawMaterialSystem::set_render_target(Context& context, RenderTarget::IdType render_target_id)
{
	render_target_ = render_target_id;
	RenderTarget& render_target = context.render_target_pool.get(render_target_);
	const TextureInstance* textures = nullptr;
	size_t number = render_target.color_textures(&textures);
	ASSERT(number != 0, "Invalid render target");
		albedo_texture_ = textures[0];
	normal_texture_ = textures[1];
	TextureInstance depth_texture;
	number = render_target.depth_texture(depth_texture);
	ASSERT(number != 0, "Invalid depth texture");
	depth_texture_ = depth_texture;
}

void GBufferDrawMaterialSystem::close()
{
}

void GBufferDrawMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void GBufferDrawMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	context.materials[id()].clear();
	clear_command_.reset();

	size_t use_cubemap = 0;
	const TextureInstance& cubemap_texture = render_context.space_grid.global_cubemap();
	if (cubemap_texture.id != Texture::invalid_id)
		use_cubemap = 1;

	size_t directional_light_index = 0;
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		int type = 0;
		size_t use_shadowmap = 0;
		TextureInstance shadowmap_texture;
		UniformBuffer::IdType light_uniform;
		// Update light data
		LightData data;
		const Light& light = render_context.lights[i].light;
		type = light.type();
		if (type != Light::directional)
			update_light_data(data, scene_context, render_context.lights[i], light);
		else
		{
			DirectionalLightData light_data;
			update_directional_light_data(light_data, scene_context, render_context.lights[i], light);
			UniformBuffer& uniform = context.uniform_pool.get(directional_light_uniform_[directional_light_index++]);
			uniform.update(light_data);
			light_uniform = uniform.id();
		}

		const ShadowInfo* shadow_info = light.shadow_info();
		if (shadowmap_enabled_.value() && shadow_info != nullptr)
		{
			use_shadowmap = 1;
			shadowmap_texture = shadow_info->shadowmap;
		}

		if (light.type() != Light::directional)
		{
			UniformBuffer& uniform = context.uniform_pool.get(light_uniform_[i]);
			uniform.update(reinterpret_cast<const uint8_t*>(&data), sizeof(LightData));
			light_uniform = uniform.id();
		}

		// Choose the correct shader
		UberShader& shader = ubershader(context);
		UberShader::Index ubershader_index;
		ubershader_index.set(shader.info("LIGHT_TYPE"), type);
		ubershader_index.set(shader.info("SHADOWMAP"), use_shadowmap);
		ubershader_index.set(shader.info("SHADOWMAP_QUALITY"), shadowmap_quality_.value());
		ubershader_index.set(shader.info("CUBEMAP"), use_cubemap);
		ShaderProgram::IdType shader_program_id = ubershader(context).get(ubershader_index);

		// Here we go - it's time to kick off something to draw
		DrawCall& draw_call = render_context.draw_calls.add();

		if (type == Light::directional)
			draw_call.render_data = quad_mesh_.mesh.parts[0].render_data;
		else if (type == Light::spot)
			draw_call.render_data = quad_mesh_.mesh.parts[0].render_data;
		else
			draw_call.render_data = sphere_mesh_.mesh.parts[0].render_data;

		Material& material = create_and_get(context.materials[id()]);
		material.shader_program = shader_program_id;
		material.textures[0] = albedo_texture_;
		material.textures[1] = normal_texture_;
		material.textures[2] = depth_texture_;
		material.textures[shadowmap_texture_unit] = shadowmap_texture;
		material.textures[env_cubemap_texture_unit] = cubemap_texture;
		material.uniforms[0] = render_context.main_camera.percamera_uniform;
		material.uniforms[1] = light_uniform;
		draw_call.material.material_system = id();
		draw_call.material.id = material.id;

		draw_call.draw_call_data = draw_call_data_;
		draw_call.command = &list_of_commands_;
	}
}

mat4x4 GBufferDrawMaterialSystem::update_light_transform(const Light& light, const vec3& position, const vec3& /*direction*/) const
{
	if (light.type() == Light::directional) return mat4x4::identity();
	if (light.type() == Light::omni)
	{
		mat4x4 res = mat4x4::scaling_matrix(light.desc().omni.radius);
		res.set_row(3, position);
		return res;
	}
	return mat4x4::identity();
}

void GBufferDrawMaterialSystem::update_light_data(LightData& light_data, const SceneContext& scene_context, 
	const LightInstance& light_instance, const Light& light) const
{
	light_data.diffuse = light.shading().diffuse * light.shading().intensity;
	light_data.diffuse.set_w(light.type() == Light::spot ? light.spot_angle_coeff() : light.angle());
	light_data.specular = light.shading().specular * light.shading().specular_intensity;
	light_data.specular.set_w(light.attenuation_b());
	const vec3& light_position = get_light_position(scene_context, light_instance.id);
	light_data.position = vec4(light_position.x(), light_position.y(), light_position.z(), light.attenuation_a());
	const vec3& light_direction = get_light_direction(scene_context, light_instance.id);
	light_data.direction = vec4(light_direction.x(), light_direction.y(), light_direction.z(), light.angle_attenuation());

	const ShadowInfo* shadow_info = light.shadow_info();
	if (shadowmap_enabled_.value() && shadow_info != nullptr)
	{
		light_data.lightvp = shadow_info->lightvp[0];
		light_data.shadowmap_params = vec4(light.desc().shadowmap_bias, 0.0f, 0.0f, 0.0f);
	}

	light_data.lightw = update_light_transform(light, light_position, light_direction);
}

void GBufferDrawMaterialSystem::update_directional_light_data(DirectionalLightData& light_data,
	const SceneContext& scene_context,
	const LightInstance& light_instance, const Light& light) const
{
	light_data.diffuse = light.shading().diffuse * light.shading().intensity;
	light_data.specular = light.shading().specular * light.shading().specular_intensity;
	light_data.direction = get_light_direction(scene_context, light_instance.id);

	const ShadowInfo* shadow_info = light.shadow_info();
	if (shadowmap_enabled_.value() && shadow_info != nullptr)
	{
		light_data.shadowmap_params = vec4(light.desc().shadowmap_bias, 0.0f, 0.0f, 0.0f);

		light_data.cascades_number = shadow_info->cascades_number;
		for (size_t i = 0; i < shadow_info->cascades_number; ++i)
		{
			light_data.csm_offset[i] = shadow_info->offset[i];
			light_data.csm_scale[i] = shadow_info->scale[i];
			light_data.cascade_znear[i] = shadow_info->znear[i];
			light_data.cascade_zfar[i] = shadow_info->zfar[i];
			light_data.lightvp[i] = shadow_info->lightview[i];
		}
	}
}

}

#include "render/gbuffer_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/light_instance_methods.hpp"
#include "render/utils/simple_meshes.hpp"

namespace mhe {

GBufferFillMaterialSystem::GBufferFillMaterialSystem()
{
	gbuffer_desc_.target = rt_readwrite;
	gbuffer_desc_.width = gbuffer_desc_.height = 0;
	gbuffer_desc_.color_datatype[0] = format_ubyte;
	gbuffer_desc_.color_datatype[1] = format_float;
	gbuffer_desc_.color_format[0] = format_rgba;
	gbuffer_desc_.color_format[1] = format_rgba32f;
	gbuffer_desc_.color_targets = 2;
	gbuffer_desc_.depth_format = format_d24f;
	gbuffer_desc_.depth_datatype = format_float;
	gbuffer_desc_.use_depth = true;
	gbuffer_desc_.use_stencil = false;
}

bool GBufferFillMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(StandartGeometryLayout::handle);

	clear_command_.set_driver(&context.driver);

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

	for (size_t i = 0; i < count; ++i)
	{
        DrawCallData& draw_call_data = context.draw_call_data_pool.get(instance_parts[i].draw_call_data);
		draw_call_data.render_target = render_target_;
		Material& material = context.materials[id()].get(instance_parts[i].material.id);
		size_t use_normalmap = material.textures[normal_texture_unit].id != Texture::invalid_id ? 1 : 0;

		UberShader::Index index;
		index.set(normalmap_info, use_normalmap);
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
    MATERIAL_UPDATE_WITH_COMMAND(context, scene_context, render_context, update, &clear_command_);
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context, MeshPartInstance* nodes, size_t count)
{
    clear_command_.reset();
	for (size_t i = 0; i < count; ++i)
	{
		Material& material = context.materials[id()].get(nodes[i].material.id);
		material.uniforms[perframe_data_unit] = render_context.main_camera.percamera_uniform;
	}
}

void GBufferFillMaterialSystem::setup_uniforms(Material& material, Context& /*context*/, SceneContext& /*scene_context*/, const MeshPartInstance& /*part*/, const ModelContext& model_context)
{
	material.uniforms[0] = transform_uniform_;
    material.uniforms[1] = model_context.transform_uniform;
}

bool GBufferDrawMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	RenderTargetDesc light_buffer_desc;
	light_buffer_desc.target = rt_readwrite;
	light_buffer_desc.color_datatype[0] = format_float;
	light_buffer_desc.color_format[0] = format_rgba32f;
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

	clear_command_.set_driver(&context.driver);

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
    desc.depth.enabled = false;
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

    for (size_t i = 0; i < render_context.lights_number; ++i)
    {
        int type = 0;
        size_t use_shadowmap = 0;
        TextureInstance shadowmap_texture;
        // Update light data
        LightData data;
        const Light& light = render_context.lights[i].light;
        data.diffuse = light.shading().diffuse;
        data.diffuse.set_w(light.type() == Light::spot ? light.spot_angle_coeff() : light.angle());
        data.specular = light.shading().specular;
				data.specular.set_w(light.attenuation_b());
        const vec3& light_position = get_light_position(scene_context, render_context.lights[i].id);
        data.position = vec4(light_position.x(), light_position.y(), light_position.z(), light.attenuation_a());
        const vec3& light_direction = get_light_direction(scene_context, render_context.lights[i].id);
        data.direction = vec4(light_direction.x(), light_direction.y(), light_direction.z(), light.angle_attenuation());
        type = light.type();

        if (shadowmap_enabled_.value() && light.shadowmap_texture().id != Texture::invalid_id)
        {
            use_shadowmap = 1;
            shadowmap_texture = light.shadowmap_texture();
            data.lightvp = light.vp();
            data.shadowmap_params = vec4(light.desc().shadowmap_bias, 0.0f, 0.0f, 0.0f);
        }

        data.lightw = update_light_transform(light, light_position, light_direction);

        UniformBuffer& uniform = context.uniform_pool.get(light_uniform_[i]);
        uniform.update(reinterpret_cast<const uint8_t*>(&data), sizeof(LightData));

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
        material.uniforms[1] = uniform.id();
        draw_call.material.material_system = id();
        draw_call.material.id = material.id;

        draw_call.draw_call_data = draw_call_data_;
        draw_call.command = &clear_command_;
    }
}

mat4x4 GBufferDrawMaterialSystem::update_light_transform(const Light& light, const vec3& position, const vec3& direction) const
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

}

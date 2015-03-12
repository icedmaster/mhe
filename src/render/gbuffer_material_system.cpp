#include "render/gbuffer_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/render_globals.hpp"
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
	StandartGeometryLayout::init(context);
	set_layout(StandartGeometryLayout::handle);

	clear_command_.set_driver(&context.driver);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;

	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
	if (!uniform.init(uniform_buffer_desc))
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
	for (size_t i = 0; i < count; ++i)
	{
        DrawCallData& draw_call_data = context.draw_call_data_pool.get(instance_parts[i].draw_call_data);
		draw_call_data.render_target = render_target_;
	}
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    MATERIAL_UPDATE_WITH_COMMAND(context, scene_context, render_context, update, &clear_command_);
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context, MeshPartInstance* nodes, size_t count)
{
	TransformSimpleData transform_data;
	transform_data.vp = render_context.vp;
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	uniform.update(transform_data);

    clear_command_.reset();

	// It may be done once, at startup
	UberShader& shader = ubershader(context);
	UberShader::Index index;
	for (size_t i = 0; i < count; ++i)
	{
		Material& material = context.materials[id()].get(nodes[i].material.id);
		size_t use_normalmap = !use_normalmaps || material.textures[normal_texture_unit].id == Texture::invalid_id ? 0 : 1;
		index.set(shader.info("NORMALMAP"), use_normalmap);
		material.shader_program = shader.get(index);
	}
}

void GBufferFillMaterialSystem::setup_uniforms(Material& material, Context& /*context*/, SceneContext& /*scene_context*/, const MeshPartInstance& /*part*/, const ModelContext& model_context)
{
	material.uniforms[0] = transform_uniform_;
    material.uniforms[1] = model_context.transform_uniform;
}

bool GBufferDrawMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	FullscreenLayout::init(context);

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
    lights_per_pass_ = lights_per_pass;
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

	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
	if (!uniform.init(uniform_buffer_desc))
		return false;

	clear_command_.set_driver(&context.driver);

    return init_mesh(context);
}

bool GBufferDrawMaterialSystem::init_mesh(Context& context)
{
    mesh_.mesh.parts.resize(1);
    mesh_.instance_parts.resize(1);
    if (!utils::create_fullscreen_quad(mesh_, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }

    mesh_.mesh.parts[0].render_data.layout = FullscreenLayout::handle;

    draw_call_data_ = context.draw_call_data_pool.create();
    mesh_.instance_parts[0].draw_call_data = draw_call_data_;
    DrawCallData& draw_call_data = context.draw_call_data_pool.get(draw_call_data_);
    draw_call_data.render_target = light_buffer_render_target_;
    draw_call_data.state = context.render_state_pool.create();
    RenderState& render_state = context.render_state_pool.get(draw_call_data.state);
    RenderStateDesc desc;
    desc.depth.enabled = false;
    desc.blend.enabled = true;
    desc.blend.dstmode = blend_one;
    desc.blend.srcmode = blend_one;
    return render_state.init(desc);
}

void GBufferDrawMaterialSystem::set_render_target(Context& context, RenderTarget::IdType render_target_id)
{
    render_target_ = render_target_id;
    RenderTarget& render_target = context.render_target_pool.get(render_target_);
    const TextureInstance* textures = nullptr;
    size_t number = render_target.color_textures(&textures);
    ASSERT(number != 0, "Invalid render target");
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

    // Transform uniform
    TransformData transform_data;
    transform_data.vp = render_context.vp;
    transform_data.inv_proj = render_context.proj.inverted();
    transform_data.inv_vp = render_context.inv_vp;
    transform_data.viewpos = render_context.viewpos;
    context.uniform_pool.get(transform_uniform_).update(transform_data);

    size_t lights_per_every_pass[max_additional_render_passes_number];
    ::memset(lights_per_every_pass, 0, sizeof(lights_per_every_pass));
    size_t passes_number = calculate_passes_number(render_context, lights_per_every_pass);
    if (passes_number == 0) return;

    LightData data[max_lights_number];
    size_t i = 0;
    for (size_t pass_number = 0; pass_number < passes_number; ++pass_number)
    {
        int type = 0;
        size_t use_shadowmap = 0;
        size_t shadowmap_light_index = 1;
        TextureInstance shadowmap_texture;
        // Update light data
        for (size_t l = 0; l < lights_per_every_pass[pass_number]; ++l, ++i)
        {
            const Light& light = render_context.lights[i].light;
            data[l].diffuse = light.shading().diffuse;
            data[l].diffuse.set_w(light.type() == Light::spot ? light.spot_angle_coeff() : light.angle());
            data[l].specular = light.shading().specular;
            const vec3& light_position = get_light_position(scene_context, render_context.lights[i].id);
            data[l].position = vec4(light_position.x(), light_position.y(), light_position.z(), light.attenuation());
            const vec3& light_direction = get_light_direction(scene_context, render_context.lights[i].id);
            data[l].direction = vec4(light_direction.x(), light_direction.y(), light_direction.z(), light.angle_attenuation());
            type = light.type();

            if (shadowmap_enabled_.value() && light.shadowmap_texture().id != Texture::invalid_id)
            {
                use_shadowmap = 1;
                shadowmap_light_index = l;
                shadowmap_texture = light.shadowmap_texture();
                data[l].lightvp = get_light_shadowmap_matrix(scene_context, render_context.lights[i].id);
                data[l].shadowmap_params = vec4(light.desc().shadowmap_bias, 0.0f, 0.0f, 0.0f);
            }
        }

        UniformBuffer& uniform = context.uniform_pool.get(light_uniform_[pass_number]);
        uniform.update(reinterpret_cast<const uint8_t*>(&data[0]), sizeof(LightData) * lights_per_every_pass[pass_number]);

        // Choose the correct shader
        UberShader& shader = ubershader(context);
        UberShader::Index ubershader_index;
        ubershader_index.set(shader.info("LIGHT_TYPE"), type);
        ubershader_index.set(shader.info("LIGHTS_NUMBER"), lights_per_every_pass[pass_number]);
        ubershader_index.set(shader.info("SHADOWMAP"), use_shadowmap);
        ubershader_index.set(shader.info("SHADOWMAP_LIGHT"), shadowmap_light_index);
        ubershader_index.set(shader.info("SHADOWMAP_QUALITY"), shadowmap_quality_.value());
        ShaderProgram::IdType shader_program_id = ubershader(context).get(ubershader_index);

        // Here we go - it's time to kick off something to draw
        DrawCall& draw_call = render_context.draw_calls.add();
        draw_call.render_data = mesh_.mesh.parts[0].render_data;

        Material& material = create_and_get(context.materials[id()]);
        material.shader_program = shader_program_id;
        material.textures[0] = normal_texture_;
        material.textures[1] = depth_texture_;
        material.textures[shadowmap_texture_unit] = shadowmap_texture;
        material.uniforms[0] = transform_uniform_;
        material.uniforms[1] = uniform.id();
        draw_call.material.material_system = id();
        draw_call.material.id = material.id;

        draw_call.draw_call_data = draw_call_data_;
        draw_call.command = &clear_command_;
    }
}

size_t GBufferDrawMaterialSystem::calculate_passes_number(RenderContext& render_context, size_t* passes) const
{
	size_t indexes[Light::light_types_number];
	::memset(indexes, 0, sizeof(indexes));
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		const Light& light = render_context.lights[i].light;
		++indexes[light.type()];
	}

	size_t idx = 0;
	size_t number = 0;
	for (size_t i = 0; i < Light::light_types_number; ++i)
	{
		size_t passes_per_type = indexes[i] / lights_per_pass_ + (indexes[i] % lights_per_pass_ == 0 ? 0 : 1);
		if (!passes_per_type) continue;
		size_t j = 0;
		for (size_t end = (passes_per_type > 1) ? passes_per_type - 1 : 0; j < end; ++j)
			passes[idx++] = lights_per_pass_;
		passes[idx++] = indexes[i] - lights_per_pass_ * j;
		number += passes_per_type;
	}
	return number;
}

}

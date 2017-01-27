#include "render/vct_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/mesh.hpp"
#include "render/renderer.hpp"
#include "utils/global_log.hpp"

namespace mhe {

const string vct_voxelize_clear_shader_name = "compute/vct_voxelize_clear";
const string vct_debug_shader_name = "vct_debug";

bool VoxelizeMaterialSystem::VoxelizedSceneVisualizer::init(VoxelizeMaterialSystem *voxelize_material_system,
                                                            Context &context)
{
    voxelize_material_system_ = voxelize_material_system;

    Shader shader;
    VERIFY(context.shader_manager.get(shader, vct_debug_shader_name), "Can't load a shader:" << vct_debug_shader_name);
    shader_program_id_ = context.ubershader_pool.get(shader.shader_program_handle).get_default();
    return utils::create_cube(cube_instance_, context);
}

void VoxelizeMaterialSystem::VoxelizedSceneVisualizer::destroy(Context &context)
{
    destroy_mesh_instance(cube_instance_, context);
}

void VoxelizeMaterialSystem::VoxelizedSceneVisualizer::before_render(Context &context,
                                                                     SceneContext &scene_context,
                                                                     RenderContext &render_context)
{
    clear_command_.reset();
    DrawCallExplicit& draw_call = render_context.explicit_draw_calls.add();
    prepare_draw_call(draw_call);
    MeshPart& part = cube_instance_.mesh.parts[0];
    draw_call.elements_number = part.render_data.elements_number;
    draw_call.ibuffer = &context.index_buffer_pool.get(part.render_data.ibuffer);
    draw_call.ibuffer_offset = 0;
    draw_call.indices_number = part.render_data.indexes_number;
    draw_call.layout = &context.layout_pool.get(part.render_data.layout);
    draw_call.pass = 0;
    draw_call.primitive = part.render_data.primitive;
    draw_call.render_command = &clear_command_;
    draw_call.render_state = &context.render_state_pool.get(context.shared.render_states.default_render_state);
    draw_call.render_target = nullptr;
    draw_call.shader_program = &context.shader_pool.get(shader_program_id_);
    draw_call.vbuffer = &context.vertex_buffer_pool.get(part.render_data.vbuffer);
    draw_call.vbuffer_offset = part.render_data.vbuffer_offset;
    draw_call.uniforms[0] = &context.uniform_pool.get(context.renderer->main_camera_uniform());
    draw_call.images[1] = &context.texture_pool.get(voxelize_material_system_->position_texture_id_);
    draw_call.images[2] = &context.texture_pool.get(voxelize_material_system_->color_texture_id_);
    draw_call.image_access[1] = access_readonly;
    draw_call.image_access[2] = access_readonly;
    const uivec3& grid_size = voxelize_material_system_->settings_.shared.dbsettings.size;
    draw_call.instances_count = grid_size.x() * grid_size.y() * grid_size.z();
}

bool VoxelizeMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!init_default(context, material_system_context))
    {
        ERROR_LOG("Material default initialization failed");
        return false;
    }

    Shader shader;
    VERIFY(context.shader_manager.get(shader, vct_voxelize_clear_shader_name), "Couldn't get a shader:" << vct_voxelize_clear_shader_name);
    clear_shader_id_ = context.ubershader_pool.get(shader.shader_program_handle).get_default();

    if (material_system_context.deserializer != nullptr)
        settings_.shared.dbsettings.read(*material_system_context.deserializer);

    TextureDesc texture_desc;
    texture_desc.address_mode_r = texture_clamp;
    texture_desc.address_mode_s = texture_clamp;
    texture_desc.address_mode_t = texture_clamp;
    texture_desc.format = format_r32ui;
    texture_desc.type = texture_buffer;
    texture_desc.width = settings_.shared.dbsettings.size.x() * settings_.shared.dbsettings.size.y() * settings_.shared.dbsettings.size.z();
    texture_desc.height = 1;
    texture_desc.depth = 1;
    {
        Texture& texture = create_and_get(context.texture_pool);
        VERIFY(texture.init(texture_desc, nullptr, texture_desc.width * 4), "Position buffer initialization failed");
        position_texture_id_ = texture.id();
    }

    texture_desc.type = texture_3d;
    texture_desc.width = settings_.shared.dbsettings.size.x();
    texture_desc.height = settings_.shared.dbsettings.size.y();
    texture_desc.depth = settings_.shared.dbsettings.size.z();

    ASSERT(texture_desc.width == texture_desc.height && texture_desc.width == texture_desc.depth, "Only the same size along all axes is now supported");

    {
        Texture& texture = create_and_get(context.texture_pool);
        VERIFY(texture.init(texture_desc, nullptr, 0), "Color texture initialization failed");
        color_texture_id_ = texture.id();
    }
    {
        Texture& texture = create_and_get(context.texture_pool);
        VERIFY(texture.init(texture_desc, nullptr, 0), "Normal texture initialization failed");
        normal_texture_id_ = texture.id();
    }

    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(UniformData);
    uniform_buffer_desc.unit = 0;
    UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
    VERIFY(uniform_buffer.init(uniform_buffer_desc), "Uniform buffer initialization failed");
    uniform_id_ = uniform_buffer.id();
    UniformData uniform_data;
    init_uniform_data(uniform_data);
    uniform_buffer.update(uniform_data);

    RenderStateDesc render_state_desc;
    render_state_desc.blend.enabled = false;
    render_state_desc.blend.color_write = false;
    render_state_desc.depth.write_enabled = false;
    render_state_desc.depth.test_enabled = false;
    render_state_desc.rasterizer.cull = cull_none;
    render_state_desc.rasterizer.depth_test_enabled = false;
    render_state_desc.scissor.enabled = false;
    render_state_desc.stencil.enabled = false;
    render_state_desc.viewport.viewport = rect<int>(0, 0, texture_desc.width, texture_desc.height);
    RenderState& render_state = create_and_get(context.render_state_pool);
    VERIFY(render_state.init(render_state_desc), "Render state initialization failed");
    render_state_id_ = render_state.id();

    VERIFY(atomic_counter_.init(format_uint), "Atomic counter initialization failed");

    // setup the clear command
    ComputeCallExplicit& compute_call = clear_draw_call_command_.compute_call();
    compute_call.shader_program = &context.shader_pool.get(clear_shader_id_);
    compute_call.images[0] = &context.texture_pool.get(color_texture_id_);
    compute_call.images[1] = &context.texture_pool.get(normal_texture_id_);
    compute_call.image_access[0] = access_writeonly;
    compute_call.image_access[1] = access_writeonly;

    const size_t threads_number = compute_call.shader_program->variable_value<size_t>("THREADS_NUMBER");
    compute_call.workgroups_number.set(settings_.shared.dbsettings.size.x() / threads_number,
        settings_.shared.dbsettings.size.y() / threads_number,
        settings_.shared.dbsettings.size.z() / threads_number);

    compute_call.barrier = memory_barrier_image_access;
    clear_draw_call_command_.set_stages(render_stage_begin_priority);

    voxels_visualizer_.init(this, context);

    return true;
}

void VoxelizeMaterialSystem::init_uniform_data(UniformData& data) const
{
    data.grid_size = vec4(settings_.shared.dbsettings.size, 0.0f);

    mat4x4 proj;
    proj.set_ortho(0.0f, static_cast<float>(settings_.shared.dbsettings.size.x()),
        0.0f, static_cast<float>(settings_.shared.dbsettings.size.y()),
        0.0f, static_cast<float>(settings_.shared.dbsettings.size.z()));
    // view matrices
    mat4x4 view;
    // +X
    view.set(0.0f, 0.0f, -1.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f, 0.0f,
             0.0f, 0.0f, -1.0f, 1.0f);
    data.vp[0] = view * proj;
    // +Y
    view.set(1.0f, 0.0f, 0.0f, 0.0f,
             0.0f, 0.0f, -1.0f, 0.0f,
             0.0f, -1.0f, 0.0f, 0.0f,
             0.0f, 0.0f, -1.0f, 1.0f);
    data.vp[1] = view * proj;
    // +Z
    view.set(-1.0f, 0.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f,
             0.0f, 0.0f, -1.0f, 0.0f,
             0.0f, 0.0f, -1.0f, 1.0f);
    data.vp[2] = view * proj;

    const uivec3 half_size = settings_.shared.dbsettings.size / 2;
    data.worldspace_to_voxelspace.set(
        1.0f / settings_.shared.dbsettings.cell_size, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / settings_.shared.dbsettings.cell_size, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / settings_.shared.dbsettings.cell_size, 0.0f,
        static_cast<float>(half_size.x()), static_cast<float>(half_size.y()), static_cast<float>(half_size.z()), 1.0f);
}

void VoxelizeMaterialSystem::destroy(Context& context)
{
    voxels_visualizer_.destroy(context);
    atomic_counter_.destroy();
    destroy_pool_object(context.render_state_pool, render_state_id_);
    destroy_pool_object(context.uniform_pool, uniform_id_);
    destroy_pool_object(context.texture_pool, normal_texture_id_);
    destroy_pool_object(context.texture_pool, color_texture_id_);
    destroy_pool_object(context.texture_pool, position_texture_id_);
}

void VoxelizeMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    ASSERT(false, "This method shouldn't be called");
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void VoxelizeMaterialSystem::init_debug_views(Context& context)
{
    context.debug_views->add_debug_scene(string("voxels"), &voxels_visualizer_);
}

void VoxelizeMaterialSystem::update(Context& context, SceneContext&, RenderContext& render_context)
{
    context.materials[id()].clear();

    atomic_counter_.set(0);

    const NodeInstance* nodes = render_context.nodes;
    const size_t nodes_count = render_context.nodes_number;
    for (size_t i = 0; i < nodes_count; ++i)
    {
        for (size_t j = 0, size = nodes[i].mesh.instance_parts.size(); j < size; ++j)
        {
            if (!nodes[i].mesh.instance_parts[j].visible) continue;
            const MeshPartInstance& part_instance = nodes[i].mesh.instance_parts[j];
            DrawCall& draw_call = render_context.draw_calls.add();
            Material& original_material = context.materials[part_instance.material.material_system].get(part_instance.material.id);
            Material& material = create_and_get(context.materials[id()]);
            material.shader_program = ubershader(context).get_default();
            material.uniforms[0] = uniform_id_;
            material.uniforms[1] = original_material.uniforms[1];
            material.textures[0] = original_material.textures[0];
            material.images[1].id = position_texture_id_;
            material.images[2].id = color_texture_id_;
            material.images[3].id = normal_texture_id_;
            setup_draw_call(draw_call, part_instance, nodes[i].mesh.mesh.parts[j], default_render_target);
            draw_call.material.id = material.id;
            draw_call.material.material_system = id();
            draw_call.render_state = render_state_id_;
            draw_call.atomics[0] = &atomic_counter_;
            draw_call.command = &clear_draw_call_command_;
            draw_call.barrier = memory_barrier_image_access | memory_barrier_atomic_counter;
        }
    }
}

}

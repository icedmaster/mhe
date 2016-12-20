#include "render/vct_material_system.hpp"

#include "render/context.hpp"
#include "utils/global_log.hpp"

namespace mhe {

bool VoxelizeMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!init_default(context, material_system_context))
    {
        ERROR_LOG("Material default initialization failed");
        return false;
    }

    if (material_system_context.deserializer != nullptr)
        settings_.shared.dbsettings.read(*material_system_context.deserializer);

    TextureBufferDesc buffer_desc;
    buffer_desc.format = format_r32ui;
    buffer_desc.unit = 0;
    buffer_desc.update_type = buffer_update_type_dynamic;
    TextureBuffer& texture_buffer = create_and_get(context.texture_buffer_pool);
    ASSERT(texture_buffer.init(buffer_desc, 0, nullptr), "Position texture buffer initialization failed");
    position_texture_id_ = texture_buffer.id();

    TextureDesc texture_desc;
    texture_desc.address_mode_r = texture_clamp;
    texture_desc.address_mode_s = texture_clamp;
    texture_desc.address_mode_t = texture_clamp;
    texture_desc.format = format_r32ui;
    texture_desc.type = texture_3d;
    texture_desc.width = settings_.shared.dbsettings.size.x();
    texture_desc.height = settings_.shared.dbsettings.size.y();
    texture_desc.depth = settings_.shared.dbsettings.size.z();
    texture_desc.type = texture_3d;
    {
        Texture& texture = create_and_get(context.texture_pool);
        ASSERT(texture.init(texture_desc, nullptr, 0), "Color texture initialization failed");
        color_texture_id_ = texture.id();
    }
    {
        Texture& texture = create_and_get(context.texture_pool);
        ASSERT(texture.init(texture_desc, nullptr, 0), "Normal texture initialization failed");
        normal_texture_id_ = texture.id();
    }

    return true;
}

void VoxelizeMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.texture_pool, normal_texture_id_);
    destroy_pool_object(context.texture_pool, color_texture_id_);
    destroy_pool_object(context.texture_pool, position_texture_id_);
}

void VoxelizeMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    ASSERT(false, "This method shouldn't be called");
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void VoxelizeMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{

}

}
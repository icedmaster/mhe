#include "res/mesh_loader.hpp"

#include "res/mhe_binary_mesh.hpp"

#include "render/context.hpp"

#include <fstream>

namespace mhe {

bool MeshLoader::load(type& res, const std::string& name, context_type* context)
{
    std::ifstream f(name.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open())
        return false;
    bool result = mhe_binary_mesh_load(res, f, context);
    f.close();
    return result;
}

bool MeshLoader::setup_instance(MeshInstance& instance, context_type* context)
{
    instance.instance_parts.resize(instance.mesh.parts.size());

    if (!instance.mesh.skeleton.bones.empty())
    {
        TextureBufferDesc texture_buffer_desc;
        texture_buffer_desc.format = format_rgba32f;
        texture_buffer_desc.update_type = buffer_update_type_dynamic;
        TextureBuffer& texture_buffer = create_and_get(context->texture_buffer_pool);
        if (!texture_buffer.init(texture_buffer_desc, sizeof(mat4x4) * instance.mesh.skeleton.bones.size(), nullptr))
        {
            context->texture_buffer_pool.remove(texture_buffer.id());
            ERROR_LOG("Can't create the texture buffer for skeletal animation");
            return false;
        }
        instance.skeleton_instance.texture_buffer = texture_buffer.id();
    }

    return true;
}

}

#include "res/material_loader.hpp"

#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "res/mhe_binary_mesh.hpp"
#include "res/scene_export.hpp"
#include "utils/global_log.hpp"

namespace mhe {

bool MaterialLoader::load(type& res, const FilePath& name, const context_type* context)
{
    mhe::res::Material dbmaterial;
    FilePath fullpath = name;
    const FilePath& ext = utils::get_file_extension(name);
    if (ext.empty()) fullpath += mhe_material_extension;
    mhe::res::XMLDeserializer deserializer(fullpath.c_str(), "material");
    if (!dbmaterial.read(deserializer))
    {
        ERROR_LOG("Couldn't load a material:" << name);
        return false;
    }

    MaterialData& material_data = create_and_get(context->material_data_pool);
    if (!dbmaterial.albedo.ref.empty())
        context->texture_manager.get(material_data.textures[albedo_texture_unit], dbmaterial.albedo.ref);
    if (!dbmaterial.normalmap.ref.empty())
        context->texture_manager.get(material_data.textures[normal_texture_unit], dbmaterial.normalmap.ref);
    if (!dbmaterial.roughness_map.ref.empty())
        context->texture_manager.get(material_data.textures[roughness_texture_unit], dbmaterial.roughness_map.ref);
    if (!dbmaterial.metalness_map.ref.empty())
        context->texture_manager.get(material_data.textures[metalness_texture_unit], dbmaterial.metalness_map.ref);

    res = material_data.id;

    return true;
}

}

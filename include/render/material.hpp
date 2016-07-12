#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "core/hash.hpp"
#include "core/hashmap.hpp"
#include "utils/pool_utils.hpp"
#include "math/vector4.hpp"
#include "texture.hpp"
#include "shader_program.hpp"
#include "render_target.hpp"
#include "render_buffer.hpp"

namespace mhe {

class Driver;
struct Context;

const size_t material_textures_number = 8;
const size_t material_uniforms_number = 4;
const size_t material_texture_buffers_number = 3;

// TODO: change ShaderProgram::IdType -> ShaderInstance
struct Material
{
    POOL_STRUCT(uint16_t);
    TextureInstance textures[material_textures_number];
    UniformBuffer::IdType uniforms[material_uniforms_number];
    TextureBuffer::IdType texture_buffers[material_texture_buffers_number];
    ShaderStorageBufferHandleType shader_storage_buffers[material_uniforms_number];
    ShaderProgram::IdType shader_program;

    Material() : shader_program(ShaderProgram::invalid_id)
    {
        ::memset(uniforms, UniformBuffer::invalid_id, sizeof(uniforms));
        ::memset(textures, Texture::invalid_id, sizeof(textures));
        ::memset(texture_buffers, TextureBuffer::invalid_id, sizeof(texture_buffers));
        ::memset(shader_storage_buffers, InvalidHandle<ShaderStorageBufferHandleType>(), sizeof(shader_storage_buffers));
    }
};

const uint8_t invalid_material_system_id = 255;

struct MaterialInstance
{
    Material::IdType id;
    uint8_t material_system;

    MaterialInstance() : id(Material::invalid_id), material_system(invalid_material_system_id) {}
};

const float default_shininess = 50.0f;
const float default_glossiness = 0.0f;

struct MaterialRenderData
{
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    vec3 emissive;
    float specular_shininess;
    float glossiness;

    MaterialRenderData() : specular_shininess(default_shininess), glossiness(default_glossiness) {}
};

enum LightingModel
{
    material_type_blinn_phong,
    material_type_ggx
};

struct MaterialData
{
    POOL_STRUCT(MaterialDataIdType);
    MaterialRenderData render_data;
    TextureInstance textures[material_textures_number];
    LightingModel lighting_model;

    MaterialData() : lighting_model(material_type_ggx) {}
};

}

#endif

#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "core/hash.hpp"
#include "utils/pool_utils.hpp"
#include "math/vector4.hpp"
#include "texture.hpp"
#include "shader_program.hpp"
#include "render_target.hpp"
#include "render_buffer.hpp"

namespace mhe {

class Driver;

const size_t material_textures_number = 8;
const size_t material_uniforms_number = 4;

// TODO: change ShaderProgram::IdType -> ShaderInstance
struct Material
{
	POOL_STRUCT(uint16_t);
	TextureInstance textures[material_textures_number];
	UniformBuffer::IdType uniforms[material_uniforms_number];
	ShaderProgram::IdType shader_program;

	Material() : shader_program(ShaderProgram::invalid_id)
	{
		::memset(uniforms, UniformBuffer::invalid_id, sizeof(uniforms));
		::memset(textures, Texture::invalid_id, sizeof(textures));
	}
};

const uint8_t invalid_material_system_id = 255;

struct MaterialInstance
{
	Material::IdType id;
	uint8_t material_system;

    MaterialInstance() : id(Material::invalid_id), material_system(invalid_material_system_id) {}
};

}

#endif

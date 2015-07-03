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

struct MaterialRenderData
{
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	vec3 emissive;
	float specular_shininess;
	float glossiness;
};

const float default_shininess = 50.0f;
const float default_glossiness = 0.0f;

struct MaterialData
{
	FilePath name;
	MaterialRenderData render_data;
	TextureInstance textures[material_textures_number];
	string lighting_model;
};

struct MaterialInitializationData
{
	FilePath name;
	MaterialRenderData render_data;
	FilePath textures[material_textures_number];
	string lighting_model;
};

typedef uint16_t MaterialId;

class MHE_EXPORT MaterialManager
{
public:
	MaterialId get(const MaterialInitializationData& data) const;

	bool get(MaterialData& material, MaterialId id) const
	{
		MaterialsMap::iterator it = materials_.find(id);
		if (it == materials_.end()) return false;
		material = it->value;
		return true;
	}

	MaterialData& material_data(MaterialId id)
	{
		ASSERT(materials_.find(id) != materials_.end(), "Invalid material id:" << id);
		return materials_[id];
	}

	bool id_by_name(MaterialId& id, const string& name) const;

	void set_context(Context* context)
	{
		context_ = context;
	}
private:
	typedef hashmap<MaterialId, MaterialData> MaterialsMap;
	Context* context_;
	mutable MaterialsMap materials_;
};

}

#endif

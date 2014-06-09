#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "utils/pool_utils.hpp"
#include "math/vec4.hpp"
#include "texture.hpp"

namespace mhe {

class Driver;

const size_t material_textures_number = 8;
const size_t material_uniforms_number = 4;

template <class MP>
class Material
{
	friend class Driver;
public:
	typedef MP Parameters;
public:
	Parameters& parameters()
	{
		return parameters_;
	}
private:
	Parameters parameters_;
	Texture::IdType textures_[material_textures_number];
	UniformBuffer::IdType uniforms_[material_uniforms_number];
	ShaderProgram::IdType shader_program_;
};

struct BlinnPhongMaterialParameters
{
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

typedef Material<BlinnPhongMaterialParameters> BlinnPhongMaterial;

typedef uint16_t material_id;

}

#endif

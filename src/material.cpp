#include "material.hpp"

namespace mhe {

Material::Material(const boost::shared_ptr<TextureAtlas>& texture_atlas, const std::string& name,
				   const boost::shared_ptr<ShaderProgram>& shader) :
	shader_(shader)
{
	uv_.push_back(texture_atlas->get(name));
	texture_atlas_.push_back(texture_atlas);
	texture_.push_back(texture_atlas->texture());
}

Material::Material(const boost::shared_ptr<Texture>& texture,
				   const boost::shared_ptr<ShaderProgram>& shader) :
	shader_(shader)
{
	add_texture(texture);
}

void Material::add_texture(const boost::shared_ptr<Texture>& texture)
{
	size_t index = texture_.size();
	texture_.push_back(texture);
	uv_.push_back(std::vector<float>());
	init_uv(index);
}

void Material::init_uv(size_t index)
{
	assert(index < uv_.size());
	uv_[index].resize(8);
	uv_[index][0] = 0; uv_[index][1] = 0;
	uv_[index][2] = 0; uv_[index][3] = 1;
	uv_[index][4] = 1; uv_[index][5] = 1;
	uv_[index][6] = 1; uv_[index][7] = 0;
}

}

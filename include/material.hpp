#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "texture_atlas.hpp"
#include "shader_program.hpp"
#include "engine_config.hpp"
#include "fixed_size_vector.hpp"

namespace mhe {

class Driver;

class Material
{
	friend class Driver;
public:
	Material(const boost::shared_ptr<TextureAtlas>& texture_atlas, const std::string& name,
			 const boost::shared_ptr<ShaderProgram>& shader) :
		shader_(shader)
	{
		uv_.push_back(texture_atlas->get(name));
		texture_atlas_.push_back(texture_atlas);
		texture_.push_back(texture_atlas->texture());
	}

	Material(const boost::shared_ptr<Texture>& texture,
			 const boost::shared_ptr<ShaderProgram>& shader) :
		shader_(shader)
	{
		init_uv(0);
		texture_.push_back(texture);
	}

	boost::shared_ptr<ShaderProgram> shader() const
	{
		return shader_;
	}

	std::vector<float> uv() const
	{
		return uv_[0];
	}

	std::vector<float> uv_at(size_t index) const
	{
		return uv_[index];
	}

	boost::shared_ptr<Texture> texture() const
	{
		return texture_[0];
	}

	boost::shared_ptr<Texture> texture_at(size_t index) const
	{
		return texture_[index];
	}

	size_t textures_number() const
	{
		return texture_.size();
	}

	bool operator== (const Material& other) const
	{
		if (shader_ != other.shader_) return false;
		if (texture_ != other.texture_) return false;
		return true;
	}

	bool operator!= (const Material& other) const
	{
		return !(*this == other);
	}
private:
	void init_uv(size_t index)
	{
		if (uv_.size() <= index)
			uv_.resize(index + 1);
		uv_[index].resize(8);
		uv_[index][0] = 0; uv_[index][1] = 0;
		uv_[index][2] = 0; uv_[index][3] = 1;
		uv_[index][4] = 1; uv_[index][5] = 1;
		uv_[index][6] = 1; uv_[index][7] = 0;
	}

	fixed_size_vector< std::vector<float>, initial_number_of_textures > uv_;
	fixed_size_vector< boost::shared_ptr<TextureAtlas>, initial_number_of_textures > texture_atlas_;	
	fixed_size_vector< boost::shared_ptr<Texture>, initial_number_of_textures > texture_;
	boost::shared_ptr<ShaderProgram> shader_;	
};

typedef boost::shared_ptr<Material> material_ptr;

}

#endif

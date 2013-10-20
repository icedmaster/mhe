#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "texture_atlas.hpp"
#include "shader_program.hpp"

namespace mhe {

class Driver;

class Material
{
	friend class Driver;
public:
	Material(const boost::shared_ptr<TextureAtlas>& texture_atlas, const std::string& name,
			 const boost::shared_ptr<ShaderProgram>& shader) :
		uv_(texture_atlas->get(name)),
		texture_atlas_(texture_atlas),
		texture_(texture_atlas->texture()),
		shader_(shader)
	{}

	Material(const boost::shared_ptr<Texture>& texture,
			 const boost::shared_ptr<ShaderProgram>& shader) :
		texture_(texture),
		shader_(shader)
	{
		init_uv();
	}

	boost::shared_ptr<ShaderProgram> shader() const
	{
		return shader_;
	}

	const std::vector<float>& uv() const
	{
		return uv_;
	}

	boost::shared_ptr<Texture> texture() const
	{
		return texture_;
	}

	bool operator== (const Material& other) const
	{
		return ( (texture_atlas_->texture() == other.texture_atlas_->texture()) &&
				 (shader_ == other.shader_) );
	}

	bool operator!= (const Material& other) const
	{
		return !(*this == other);
	}
private:
	void init_uv()
	{
		uv_.resize(8);
		uv_[0] = 0; uv_[1] = 0;
		uv_[2] = 0; uv_[3] = 1;
		uv_[4] = 1; uv_[5] = 1;
		uv_[6] = 1; uv_[7] = 0;
	}

	std::vector<float> uv_;
	boost::shared_ptr<TextureAtlas> texture_atlas_;	
	boost::shared_ptr<Texture> texture_;
	boost::shared_ptr<ShaderProgram> shader_;	
};

typedef boost::shared_ptr<Material> material_ptr;

}

#endif

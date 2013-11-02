#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "texture_atlas.hpp"
#include "shader_program.hpp"
#include "engine_config.hpp"
#include "fixed_size_vector.hpp"

namespace mhe {

class Driver;

enum BlendMode
{
	no_blend,
	alpha_one_minus_alpha,
};

class Material
{
	friend class Driver;
public:
	enum
	{
		default_render = 0,
		mask_z_buffer = 1,
		lighting_disabled = (1 << 1),
		blending_enabled = (1 << 2)
	};
public:
	Material(const boost::shared_ptr<TextureAtlas>& texture_atlas, const std::string& name,
			 const boost::shared_ptr<ShaderProgram>& shader);

	Material(const boost::shared_ptr<Texture>& texture,
			 const boost::shared_ptr<ShaderProgram>& shader);

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

	void add_texture(const boost::shared_ptr<Texture>& texture);

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

		void clear_render_flags()
	{
		render_flags_ = default_render;
	}

	void set_mask_z_buffer()
	{
		render_flags_ |= mask_z_buffer;
	}

	void set_unmask_z_buffer()
	{
		render_flags_ &= ~mask_z_buffer;
	}

	bool is_z_buffer_masked() const
	{
		return render_flags_ & mask_z_buffer;
	}

	void set_lightning_enabled()
	{
		render_flags_ &= ~lighting_disabled;
	}

	void set_lighting_disabled()
	{
		render_flags_ |= lighting_disabled;
	}

	bool is_lighting_enabled() const
	{
		return !(render_flags_ & lighting_disabled);
	}

	void set_blending_enabled()
	{
		render_flags_ |= blending_enabled;
	}

	void set_blending_disabled()
	{
		render_flags_ &= ~blending_enabled;
	}

	bool is_blending_enabled() const
	{
		return render_flags_ & blending_enabled;
	}

	uint32_t render_flags() const
	{
		return render_flags_;
	}

	void set_blend_mode(BlendMode mode)
	{
		blend_mode_ = mode;
	}

	BlendMode blend_mode() const
	{
		return blend_mode_;
	}
private:
	void init_uv(size_t index);

	fixed_size_vector< std::vector<float>, initial_number_of_textures > uv_;
	fixed_size_vector< boost::shared_ptr<TextureAtlas>, initial_number_of_textures > texture_atlas_;	
	fixed_size_vector< boost::shared_ptr<Texture>, initial_number_of_textures > texture_;
	boost::shared_ptr<ShaderProgram> shader_;
	uint32_t render_flags_;
	BlendMode blend_mode_;	
};

typedef boost::shared_ptr<Material> material_ptr;

}

#endif

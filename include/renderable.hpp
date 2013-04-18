#ifndef __RENDERABLE_HPP__
#define __RENDERABLE_HPP__

#include <vector>
#include "types.hpp"
#include "texture.hpp"
#include "transform.hpp"

namespace mhe {

enum BlendMode
{
	no_blend,
	alpha_one_minus_alpha,
};

const std::string vertex_position_attribute_name = "position";
const std::string vertex_normal_attribute_name = "normals";
const std::string vertex_color_attribute_name = "color";
const std::string vertex_texcoord_attribute_name = "texcoord";

class Renderable : public Transform
{
	static const cmn::uint default_texcoord_size = 8;
public:
	enum
	{
		default_render = 0, 
		mask_z_buffer = 1,
		lighting_disabled = (1 << 1),
		blending_enabled = (1 << 2),
		batching_disabled = (1 << 3)
	};
public:
	Renderable(bool default_initialization = false) :
		color_(color_white), render_flags_(0), blend_mode_(no_blend)
	{ 
		if (default_initialization)
		{
			texcoord_.resize(default_texcoord_size);
			texcoord_[0] = 0.0; texcoord_[1] = 0.0; texcoord_[2] = 0.0; texcoord_[3] = 1.0;
			texcoord_[4] = 1.0; texcoord_[5] = 1.0; texcoord_[6] = 1.0; texcoord_[7] = 0.0;
		}
	}

	void set_texture(const boost::shared_ptr<Texture>& texture)
	{
		texture_ = texture;
	}

	const boost::shared_ptr<Texture> texture() const
	{
		return texture_;
	}

	void set_color(const colorf& color)
	{
		color_ = color;
		update_color_buffer();
	}

	const colorf& color() const
	{
		return color_;
	}

	const std::vector<float>& texcoord() const
	{
		return texcoord_;
	}

	void set_texcoord(const std::vector<float>& coord)
	{
		texcoord_ = coord;
	}

	const std::vector<float>& vertexcoord() const
	{
		return vertexcoord_;
	}

	const std::vector<float>& normalscoord() const
	{
		return normalscoord_;
	}

	const std::vector<float>& colorcoord() const
	{
		return colorcoord_;
	}

	const std::vector<cmn::uint> indicies() const
	{
		return indicies_;
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

	void set_batching_disabled()
	{
		render_flags_ |= batching_disabled;
	}

	void set_batching_enabled()
	{
		render_flags_ &= ~batching_disabled;
	}

	bool is_batching_disabled() const
	{
		return (render_flags_ & batching_disabled);
	}

	void set_blend_mode(BlendMode mode)
	{
		blend_mode_ = mode;
	}

	BlendMode blend_mode() const
	{
		return blend_mode_;
	}

	void attach(const Renderable& other)
	{
		texcoord_.insert(texcoord_.end(), other.texcoord_.begin(), other.texcoord_.end());
		size_t v_sz = vertexcoord_.size() / 3;
		for (size_t i = 0; i < other.vertexcoord_.size(); i += 3)
		{
			v3d v(other.vertexcoord_[i], other.vertexcoord_[i + 1], other.vertexcoord_[i + 2]);
			v = v * other.transform();
			vertexcoord_.insert(vertexcoord_.end(), v.get(), v.get() + 3);
			colorcoord_.insert(colorcoord_.end(), other.color().get(), other.color().get() + 4);
		}
		normalscoord_.insert(normalscoord_.end(), other.normalscoord_.begin(), other.normalscoord_.end());	
		for (size_t i = 0; i < other.indicies_.size(); ++i)
			indicies_.push_back(other.indicies_[i] + v_sz); 
		render_flags_ |= other.render_flags_;
		blend_mode_ = other.blend_mode_;
	}
protected:
	std::vector<float>& rtexcoord()
	{
		return texcoord_;
	}

	std::vector<float>& rvertexcoord()
	{
		return vertexcoord_;
	}

	void set_vertexcoord(const std::vector<float>& coord)
	{
		vertexcoord_ = coord;
	}

	std::vector<float>& rnormalscoord()
	{
		return normalscoord_;
	}

	void set_normalscoord(const std::vector<float>& coord)
	{
		normalscoord_ = coord;
	}

	std::vector<cmn::uint>& rindicies()
	{
		return indicies_;
	}

	void set_indicies(const std::vector<cmn::uint>& coord)
	{
		indicies_ = coord;
	}

	std::vector<float>& rcolorcoord()
	{
		return colorcoord_;
	}

	void update_color_buffer()
	{
		int count = vertexcoord_.size() / 3;
		colorcoord_.resize(count * 4);
		for (int i = 0; i < count * 4; i += 4)
		{
			colorcoord_[i] = color_.r();
			colorcoord_[i + 1] = color_.g();
			colorcoord_[i + 2] = color_.b();
			colorcoord_[i + 3] = color_.a();
		}
	}
private:
	boost::shared_ptr<Texture> texture_;
	std::vector<float> texcoord_;
	std::vector<float> vertexcoord_;
	std::vector<float> colorcoord_;
	std::vector<float> normalscoord_;
	std::vector<cmn::uint> indicies_;
	colorf color_;
	uint32_t render_flags_;
	BlendMode blend_mode_;
};

}

#endif

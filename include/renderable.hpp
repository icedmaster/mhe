#ifndef __RENDERABLE_HPP__
#define __RENDERABLE_HPP__

#include <vector>
#include "types.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "fixed_size_vector.hpp"
#include "engine_config.hpp"

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

template
<size_t number_of_verteces = initial_number_of_verteces,
 size_t number_of_indexes = initial_number_of_verteces,
 size_t number_of_texcoords = initial_number_of_texcoords
>
class RenderableBase : public Transform
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
	typedef fixed_size_vector<float, number_of_verteces> vertex_container;
	typedef fixed_size_vector<unsigned int, number_of_indexes> indexes_container;
	typedef fixed_size_vector<float, number_of_texcoords> texcoord_container;
public:
	RenderableBase(bool default_initialization = false) :
		color_(color_white), render_flags_(0), blend_mode_(no_blend)
	{ 
		if (default_initialization)
		{
			texcoord_[0] = 0.0; texcoord_[1] = 0.0; texcoord_[2] = 0.0; texcoord_[3] = 1.0;
			texcoord_[4] = 1.0; texcoord_[5] = 1.0; texcoord_[6] = 1.0; texcoord_[7] = 0.0;
		}
	}

	RenderableBase(size_t vertexes_number, size_t indexes_number, size_t texcoord_number) :
		color_(color_white), render_flags_(0), blend_mode_(no_blend)
	{
		vertexcoord_.reserve(vertexes_number);
		normalscoord_.reserve(vertexes_number);
		colorcoord_.reserve(vertexes_number);
		indicies_.reserve(indexes_number);
		texcoord_.reserve(texcoord_number);
	}

	void set_texture(const boost::shared_ptr<Texture>& texture)
	{
		texture_ = texture;
		on_texture_changed();
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

	const texcoord_container& texcoord() const
	{
		return texcoord_;
	}

	void set_texcoord(const texcoord_container& coord)
	{
		texcoord_ = coord;
	}

	void set_buffers(const vertex_container& vertexes, const texcoord_container& texturecoord,
					 const indexes_container& indicies)
	{
		vertexcoord_ = vertexes;
		texcoord_ = texturecoord;
		indicies_ = indicies;
		update_color_buffer();
	}

	const vertex_container& vertexcoord() const
	{
		return vertexcoord_;
	}

	const vertex_container& normalscoord() const
	{
		return normalscoord_;
	}

	const vertex_container& colorcoord() const
	{
		return colorcoord_;
	}

	const indexes_container& indicies() const
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

	template <size_t VC, size_t IC, size_t TC>
	void attach(const RenderableBase<VC, IC, TC>& other)
	{
		texcoord_.insert(texcoord_.end(), other.texcoord().begin(), other.texcoord().end());
		size_t v_sz = vertexcoord_.size() / 3;
		for (size_t i = 0; i < other.vertexcoord().size(); i += 3)
		{
			v3d v(other.vertexcoord()[i], other.vertexcoord()[i + 1], other.vertexcoord()[i + 2]);
			v = v * other.transform();
			vertexcoord_.insert(vertexcoord_.end(), v.get(), v.get() + 3);
			colorcoord_.insert(colorcoord_.end(), other.color().get(), other.color().get() + 4);
		}
		normalscoord_.insert(normalscoord_.end(), other.normalscoord().begin(), other.normalscoord().end());	
		for (size_t i = 0; i < other.indicies().size(); ++i)
			indicies_.push_back(other.indicies()[i] + v_sz); 
		render_flags_ |= other.render_flags();
		blend_mode_ = other.blend_mode();
	}

	void clear()
	{
		color_ = color_white;
		render_flags_ = 0;
		blend_mode_ = no_blend;
		texcoord_.clear();
		vertexcoord_.clear();
		normalscoord_.clear();
		colorcoord_.clear();
		indicies_.clear();
	}
protected:
	texcoord_container& rtexcoord()
	{
		return texcoord_;
	}

	vertex_container& rvertexcoord()
	{
		return vertexcoord_;
	}

	void set_vertexcoord(const vertex_container& coord)
	{
		vertexcoord_ = coord;
	}

	vertex_container& rnormalscoord()
	{
		return normalscoord_;
	}

	void set_normalscoord(const vertex_container& coord)
	{
		normalscoord_ = coord;
	}

	indexes_container& rindicies()
	{
		return indicies_;
	}

	void set_indicies(const indexes_container& coord)
	{
		indicies_ = coord;
	}

	vertex_container& rcolorcoord()
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
	virtual void on_texture_changed() {}

	texcoord_container texcoord_;
	vertex_container vertexcoord_;
	vertex_container colorcoord_;
	vertex_container normalscoord_;
	indexes_container indicies_;
	boost::shared_ptr<Texture> texture_;
	colorf color_;
	uint32_t render_flags_;
	BlendMode blend_mode_;
};

typedef RenderableBase<> Renderable;

inline Renderable::vertex_container vector_to_vertex_container(const std::vector<float>& v)
{
	Renderable::vertex_container result(v.begin(), v.end());
	return result;
}

inline Renderable::texcoord_container vector_to_texcoord_container(const std::vector<float>& v)
{
	Renderable::texcoord_container result(v.begin(), v.end());
	return result;
}

}

#endif

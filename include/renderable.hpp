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

class Renderable : public Transform
{
	static const cmn::uint default_texcoord_size = 8;
public:
	Renderable(bool default_initialization = false) :
		render_flags_(0), blend_mode_(no_blend)
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
	}

	const colorf& color() const
	{
		return color_;
	}

	const std::vector<float>& texcoord() const
	{
		return texcoord_;
	}

	const std::vector<float>& vertexcoord() const
	{
		return vertexcoord_;
	}

	const std::vector<float>& normalscoord() const
	{
		return normalscoord_;
	}

	const std::vector<cmn::uint> indicies() const
	{
		return indicies_;
	}

	void set_flags(uint32_t flags)
	{
		render_flags_ = flags;
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

	void attach(const Renderable& other)
	{
		texcoord_.insert(texcoord_.end(), other.texcoord_.begin(), other.texcoord_.end());
		for (cmn::uint i = 0; i < other.vertexcoord_.size(); i += 3)
		{
			v3d v(other.vertexcoord_[i], other.vertexcoord_[i + 1], other.vertexcoord_[i + 2]);
			v = v * other.get_transform();
			vertexcoord_.insert(vertexcoord_.end(), v.get(), v.get() + 3);
		}
		normalscoord_.insert(normalscoord_.end(), other.normalscoord_.begin(), other.normalscoord_.end());	
		size_t indicies_count = indicies_.size();
		for (size_t i = 0; i < other.indicies_.size(); ++i)
			indicies_.push_back(other.indicies_[i] + indicies_count); 
	}
protected:
	std::vector<float>& rtexcoord()
	{
		return texcoord_;
	}

	void set_texcoord(const std::vector<float>& coord)
	{
		texcoord_ = coord;
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

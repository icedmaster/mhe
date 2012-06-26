#ifndef _TEXTURE_ATLAS_HPP_
#define _TEXTURE_ATLAS_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <vector>
#include "texture.hpp"

namespace mhe {

class TextureAtlas
{
public:
	void set_texture(const boost::shared_ptr<Texture>& tex, int sz = 0);
	const boost::shared_ptr<Texture>& texture() const
	{
		return texture_;
	}
			
	void set_indicies(const float* ind, int sz);									
	Texture::texcoord get(cmn::uint quad) const;
	Texture::texcoord get(const rect<float>& quad) const;
	v2d get_size(cmn::uint quad) const;
	boost::shared_ptr<Texture> create_texture(cmn::uint quad) const;
	boost::shared_ptr<Texture> create_texture(const rect<float>& coord) const;
private:
	void calc_indicies(int sz);

	boost::shared_ptr<Texture> texture_;
	std::vector<float> indicies_;		   
};

}

#endif

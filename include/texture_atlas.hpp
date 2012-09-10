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
	std::vector<float> get(cmn::uint quad) const;
	std::vector<float> get(const rect<float>& quad) const;
	v2d get_size(cmn::uint quad) const;
private:
	void calc_indicies(int sz);

	boost::shared_ptr<Texture> texture_;
	std::vector<float> indicies_;		   
};

}

#endif

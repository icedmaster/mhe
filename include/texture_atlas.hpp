#ifndef _TEXTURE_ATLAS_HPP_
#define _TEXTURE_ATLAS_HPP_

#include "itexture.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <vector>

namespace mhe
{
class TextureAtlas
{
private:
	boost::shared_ptr<iTexture> texture_;
	std::vector<float> indicies_;
			
	void calc_indicies(int sz);
public:
	void setTexture(const boost::shared_ptr<iTexture>& tex, int sz = 0);
	const boost::shared_ptr<iTexture>& getTexture() const
	{
		return texture_;
	}
			
	void setIndicies(const float* ind, int sz);									
	iTexture::texcoord get(cmn::uint quad) const;
	iTexture::texcoord get(const rect<float>& quad) const;
	v2d get_size(cmn::uint quad) const;
	boost::shared_ptr<iTexture> createTexture(cmn::uint quad) const;
	boost::shared_ptr<iTexture> createTexture(const rect<float>& coord) const;
	};
}

#endif

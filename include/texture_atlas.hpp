#ifndef _TEXTURE_ATLAS_HPP_
#define _TEXTURE_ATLAS_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>
#include "texture.hpp"

namespace mhe {

class TextureAtlas
{
public:
	TextureAtlas()
	{}
	TextureAtlas(const boost::shared_ptr<Texture>& tex, int sz = 0);
	void set_texture(const boost::shared_ptr<Texture>& tex, int sz = 0);
	const boost::shared_ptr<Texture>& texture() const
	{
		return texture_;
	}

	void add(const std::string& name, const rect<float>& geom)
	{
		elements_[name] = geom;
	}
			
	void set_indicies(const float* ind, int sz);		
	void add_indicies(const float* ind, int sz); 
	std::vector<float> get(cmn::uint quad) const;
	std::vector<float> get(const rect<float>& quad) const;
	std::vector<float> get(const std::string& name) const;
	v2d get_size(cmn::uint quad) const;
	rect<float> get_rect(const std::string& name) const;
private:
	void calc_indicies(int sz);

	boost::shared_ptr<Texture> texture_;
	std::vector<float> indicies_;
	std::map< std::string, rect<float> > elements_;
};

}

#endif

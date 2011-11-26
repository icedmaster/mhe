#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "itexture.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
class OpenGLDriver;

class Texture : public iTexture
{
private:
	cmn::uint w_, h_;
	cmn::uint id_;	
	texcoord coord_;
	bool binded_;
	void rebuild_texture(const boost::shared_ptr<Image>& im,
						 boost::shared_ptr<iDriver> driver, FilterType ft);	

	friend class OpenGLDriver;
public:
	Texture();
	~Texture();
			
	void setImage(const boost::shared_ptr<Image>& im, boost::shared_ptr<iDriver> driver,
				  FilterType ft = Nearest)
	{
		rebuild_texture(im, driver, ft);
	}
			
	void prepare(boost::shared_ptr<iDriver> driver = 
				 boost::shared_ptr<iDriver>());
	void clean(boost::shared_ptr<iDriver> driver =
			   boost::shared_ptr<iDriver>());
			
	cmn::uint width() const
	{
		return w_;
	}
			
	cmn::uint height() const
	{
		return h_;
	}

	TextureType type() const
	{
		return Texture2D;
	}

	boost::shared_ptr<iTexture> clone() const;

	texcoord get_coord() const
	{
		return coord_;
	}

	void set_coord(const texcoord& tc)
	{
		coord_ = tc;
	}
};
}

#endif

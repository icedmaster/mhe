#ifndef __OPENGL_TEXTURE_HPP__
#define __OPENGL_TEXTURE_HPP__

#include "texture.hpp"
#include "mhe_gl.hpp"

namespace mhe {

class Driver;

namespace opengl {

class OpenGLTexture : public Texture
{
private:
	cmn::uint w_, h_;
	cmn::uint id_;	
	texcoord coord_;
	bool binded_;
	void rebuild_texture(boost::shared_ptr<Image> im,
						 boost::shared_ptr<Driver> driver, FilterType ft);	

	friend class OpenGLDriver;
public:
	OpenGLTexture();
	~OpenGLTexture();
			
	void set_image(boost::shared_ptr<Image> im, boost::shared_ptr<Driver> driver,
				   FilterType ft = Nearest)
	{
		rebuild_texture(im, driver, ft);
	}

	void prepare(boost::shared_ptr<Driver> driver = 
				 boost::shared_ptr<Driver>());
	void clean(boost::shared_ptr<Driver> driver =
			   boost::shared_ptr<Driver>());
			
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

	boost::shared_ptr<Texture> clone() const;

	texcoord get_coord() const
	{
		return coord_;
	}

	void set_coord(const texcoord& tc)
	{
		coord_ = tc;
	}
};

}}

#endif

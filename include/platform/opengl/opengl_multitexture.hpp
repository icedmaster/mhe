#ifndef _MULTITEXTURE_HPP_
#define _MULTITEXTURE_HPP_

#include "opengl_texture.hpp"

namespace mhe {
namespace opengl {

class OpenGLMultiTexture : public Texture
{
public:
	void set_image(boost::shared_ptr<Image> image,
				   boost::shared_ptr<Driver> driver, FilterType ft = Nearest);

	void set_color(const colorf& /*color*/) {}

	void prepare(boost::shared_ptr<Driver> driver = 
				 boost::shared_ptr<Driver>());
	void clean(boost::shared_ptr<Driver> driver =
			   boost::shared_ptr<Driver>());

	boost::shared_ptr<Texture> clone() const;

	cmn::uint width() const
	{
		return textures_[0]->width();
	}

	cmn::uint height() const
	{
		return textures_[0]->height();
	}

	TextureType type() const
	{
		return Multitexture;
	}

	// specific methods
	boost::shared_ptr<OpenGLTexture> get(size_t index) const
	{
		assert(index < textures_.size());
		return textures_[index];
	}

	// TODO:
	bool is_equal(const Texture& /*other*/) const
	{
		return false;
	}
private:
	std::vector< boost::shared_ptr<OpenGLTexture> > textures_;
};

}}

#endif

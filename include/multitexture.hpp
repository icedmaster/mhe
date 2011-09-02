#ifndef _MULTITEXTURE_HPP_
#define _MULTITEXTURE_HPP_

#include "texture.hpp"

namespace mhe
{
class MultiTexture : public iTexture
{
	void setImage(const boost::shared_ptr<Image>& image, FilterType ft = Nearest);
	void prepare();
	void clean();

	boost::shared_ptr<iTexture> clone() const;

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
	boost::shared_ptr<Texture> get(size_t index) const
	{
		assert(index < textures_.size());
		return textures_[index];
	}
private:
	std::vector< boost::shared_ptr<Texture> > textures_;
};
};

#endif

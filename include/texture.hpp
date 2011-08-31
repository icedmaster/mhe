#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "itexture.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
class Texture : public iTexture
{
private:
	cmn::uint w_, h_;
	cmn::uint id_;	
	texcoord coord_;
	bool binded_;
	void rebuild_texture(const boost::shared_ptr<Image>& im, FilterType ft);
public:
	Texture();
	~Texture();
			
	void setImage(const boost::shared_ptr<Image>& im, FilterType ft = Nearest)
	{
		rebuild_texture(im, ft);
	}
			
	void prepare();
	void clean();
			
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

#ifndef _ITEXTURE_HPP_
#define _ITEXTURE_HPP_

#include "image.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	enum FilterType
	{
		Nearest
	};

	enum TextureType
	{
		Texture2D,
		Multitexture
	};

	class iTexture
	{
		public:
			virtual void setImage(const boost::shared_ptr<Image>&, FilterType ft = Nearest) = 0;
			virtual void prepare() = 0;
			virtual void clean() = 0;

			virtual cmn::uint width() const = 0;
			virtual cmn::uint height() const = 0;
			virtual TextureType type() const = 0;
	};
}

#endif

#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "image.hpp"
#include "video_driver.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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

	class Texture
	{
	public:
		typedef boost::array<float, 8> texcoord;
	public:
		virtual void set_image(boost::shared_ptr<Image>, boost::shared_ptr<Driver>,
							   FilterType ft = Nearest) = 0;
		virtual void prepare(boost::shared_ptr<Driver> driver =
							 boost::shared_ptr<Driver>()) = 0;
		virtual void clean(boost::shared_ptr<Driver> driver = 
						   boost::shared_ptr<Driver>()) = 0;

		virtual boost::shared_ptr<Texture> clone() const = 0;

		virtual cmn::uint width() const = 0;
		virtual cmn::uint height() const = 0;
		virtual TextureType type() const = 0;		   

		virtual texcoord get_coord() const
		{
			texcoord tc = {{0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0}};
			return tc;
		}

		virtual void set_coord(const texcoord&) {}
	};
}

#endif

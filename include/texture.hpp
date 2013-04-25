#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "image.hpp"
#include "video_driver.hpp"
#include "utils/noncopyable.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

namespace mhe {

enum FilterType
{
	Nearest
};

enum TextureType
{
	Texture2D,
	Multitexture
};

class Texture : public utils::noncopyable
{
public:
	virtual ~Texture() {}
	// create 2x2 (pot) texture with color
	virtual void set_color(const colorf& color) = 0;
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
	virtual bool is_equal(const Texture& other) const = 0;
};

}

#endif

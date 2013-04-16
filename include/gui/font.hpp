#ifndef __FONT_HPP__ 
#define __FONT_HPP__

#include <string>
#include <boost/shared_ptr.hpp>
#include "mhe_math.hpp"
#include "video_driver.hpp"
#include "utils/utf8_utils.hpp"

namespace mhe {
namespace gui {

class Font
{
public:
	virtual ~Font() {}
	virtual bool load(const std::string& filename) = 0;
	virtual void print(const boost::shared_ptr<Driver>&, const utf32_string&,
					   const vector2<float>&, const colorf& color = color_white) = 0;
	virtual std::string name() const = 0;
	virtual cmn::uint height() const = 0;
};

}}

#endif



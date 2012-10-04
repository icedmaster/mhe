#ifndef __FONT_HPP__
#define __FONT_HPP__

#include <string>
#include <boost/shared_ptr.hpp>
#include "mhe_math.hpp"
#include "video_driver.hpp"

namespace mhe {
namespace gui {

class Font
{
public:
	virtual ~Font() {}
	virtual bool load(const std::string& filename) = 0;
	virtual void print(const boost::shared_ptr<Driver>&, const std::wstring&,
					   const vector2<int>&, const colorf& color = color_white) = 0;
	virtual std::string name() const = 0;
};

}}

#endif

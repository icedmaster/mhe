#ifndef __FONT_LOADER_HPP__
#define __FONT_LOADER_HPP__

#include <string>
#include "font.hpp"

namespace mhe {
namespace gui {

struct FontLoader
{
	typedef Font type;
	typedef Context helper_type;
	static type* load(const std::string& name, const helper_type& helper);
};

}}

#endif

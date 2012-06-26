#ifndef __FONT_LOADER_HPP__
#define __FONT_LOADER_HPP__

#include <string>

namespace mhe {
namespace gui {

class Font;

struct FontLoader
{
	typedef Font type;
	typedef void* helper_type;
	static type* load(const std::string& name, const helper_type& helper);
};

}}

#endif

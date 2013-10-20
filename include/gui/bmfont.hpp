#ifndef __BMFONT_HPP__
#define __BMFONT_HPP__

#include "texture_font.hpp"

namespace mhe {
namespace gui {

class BMFont : public TextureFont
{
private:
	bool load_font_parameters(std::istream& stream, std::string& texture_filename);
	bool load_font_chars(std::istream& stream);
	void load_char(const std::string& str);
};

}
}

#endif

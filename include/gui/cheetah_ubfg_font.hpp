#ifndef __CHEETAH_UBFG_FONT_HPP__
#define __CHEETAH_UBFG_FONT_HPP__

#include "texture_font.hpp"

namespace mhe {
namespace gui {

class UBFGFont : public TextureFont
{
private:
	bool load_font_parameters(std::istream& stream, std::string& texture_filename);
	bool load_font_chars(std::istream& stream);

	void parse_char(const std::string& str);
};

}}

#endif

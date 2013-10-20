#include "gui/bmfont.hpp"

#include <fstream>
#include "utils/options_parser.hpp"
#include "utils/logutils.hpp"
#include "texture_manager.hpp"

namespace mhe {
namespace gui {

bool BMFont::load_font_parameters(std::istream& stream, std::string& texture_filename)
{
	std::string str;
	// first string - info
	std::getline(stream, str);
	mhe::utils::OptionsParser op(str, ' ');
	std::string name;
	op.get_string("face", name, "");
	set_name(utils::replace(name, '"')); // get font name
	cmn::uint size = 0;
	op.get<cmn::uint>("size", size, 0);
	set_height(size);
	// second string - draw info
	std::getline(stream, str);
	// third string - texture info
	std::getline(stream, str);
	op.process(str, ' ');
	op.get_string("file", texture_filename, "");
	if (texture_filename.empty())
		return false;
	texture_filename = utils::replace(texture_filename, '"');
	return true;
}

bool BMFont::load_font_chars(std::istream& stream)
{
	// forth string - number of chars
	std::string str;
	std::getline(stream, str);
	utils::OptionsParser op(str, ' ');
	int chars_num;
	op.get_integer("count", chars_num, 0);

	for (int i = 0; i < chars_num; ++i)
	{
		std::getline(stream, str);
		load_char(str);
	}

	return true;
}

void BMFont::load_char(const std::string& str)
{
	mhe::utils::OptionsParser op(str, ' ');
	unsigned int id, width, height, x, y;
	op.get<unsigned int>("id", id, 0);
	op.get<unsigned int>("x", x, 0);
	op.get<unsigned int>("y", y, 0);
	op.get<unsigned int>("width", width, 0);
	op.get<unsigned int>("height", height, 0);

	add_char_for_index(id, rect<float>(x, y, width, height));
}

}}

#include "gui/cheetah_ubfg_font.hpp"

#include "utils/options_parser.hpp"
#include "utils/strutils.hpp"

namespace mhe {
namespace gui {

bool UBFGFont::load_font_parameters(std::istream& stream, std::string& texture_filename)
{
	std::string str;
	std::getline(stream, str);
	// first string - texture filename
	const std::vector<std::string>& split_res = utils::split(str, ":");
	texture_filename = utils::trim_copy(split_res[1]);
	if (texture_filename.empty()) return false;
	// second line - font name and size
	// TODO: don't split this string now
	std::getline(stream, str);
	set_name(str);
	return true;
}

bool UBFGFont::load_font_chars(std::istream& stream)
{
	std::string str;
	while (!stream.eof())
	{
		std::getline(stream, str);
		parse_char(str);
	}

	return true;
}

void UBFGFont::parse_char(const std::string& str)
{
	const std::string delimiter = "\t";
	const size_t parameters_number = 9;
	if (str.empty()) return;
	const std::vector<std::string>& params = utils::split(str, delimiter);
	if (params.size() < parameters_number) return;
	utf32_char id = utils::to_number(params[0]);
	float x = static_cast<float>(utils::to_number(params[1]));
	float y = static_cast<float>(utils::to_number(params[2]));
	float width = static_cast<float>(utils::to_number(params[3]));
	float height = static_cast<float>(utils::to_number(params[4]));
	add_char_for_index(id, rect<float>(x, y, width, height));
}

}}

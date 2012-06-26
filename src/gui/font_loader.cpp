#include "gui/font_loader.hpp"
#include "utils/file_utils.hpp"
#include "gui/bmfont.hpp"

namespace mhe {
namespace gui {

Font* FontLoader::load(const std::string& name, const FontLoader::helper_type&)
{
	const std::string& ext = utils::get_file_extension(name);
	if (ext.empty()) return nullptr;

	if (ext == "fnt")
	{
		BMFont* font = new BMFont;
		if (!font->load(name))
		{
			delete font;
			return nullptr;
		}
		return font;
	}
	return nullptr;
}

}}

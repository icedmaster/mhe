#include "gui/font_loader.hpp"
#include "utils/file_utils.hpp"
#include "gui/bmfont.hpp"
#include "gui/cheetah_ubfg_font.hpp"

namespace mhe {
namespace gui {

Font* FontLoader::load(const std::string& name, const FontLoader::helper_type& helper)
{
	const std::string& ext = utils::get_file_extension(name);
	if (ext.empty()) return nullptr;

	if (ext == "bmfnt")
	{
		BMFont* font = new BMFont;
		if (!font->load(name, helper))
		{
			delete font;
			return nullptr;
		}
		return font;
	}
	else if (ext == "fnt")
	{
		UBFGFont* font = new UBFGFont;
		if (!font->load(name, helper))
		{
			delete font;
			return nullptr;
		}
		return font;
	}
	return nullptr;
}

}}

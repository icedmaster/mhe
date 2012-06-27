#ifndef __XML_FONT_LOADER_HPP__
#define __XML_FONT_LOADER_HPP__

#include "utils/loader_utils.hpp"
#include "font_manager.hpp"

namespace mhe {
namespace gui {

class XMLFontLoader
{
public:
	XMLFontLoader(const std::string& filename,
				  FontManager& font_manager);
	XMLFontLoader(const pugi::xml_node& node,
				  FontManager& font_manager);

	boost::shared_ptr<Font> load_font(const std::wstring& font_name) const;
	void load_all_fonts();
private:
	pugi::xml_document doc_;
	pugi::xml_node root_;
	FontManager& font_manager_;
};

}}

#endif

#include "gui/xml_font_loader.hpp"

namespace mhe {
namespace gui {

XMLFontLoader::XMLFontLoader(const std::string& filename,
							 FontManager& font_manager) :
	font_manager_(font_manager)
{
	pugi::xml_parse_result parse_res = doc_.load_file(filename.c_str());
	if (parse_res.status != pugi::status_ok) return;
	root_ = doc_.child(L"fonts");
}

XMLFontLoader::XMLFontLoader(const pugi::xml_node& node,
							 FontManager& font_manager) :
	root_(node),
	font_manager_(font_manager)
{
}

boost::shared_ptr<Font> XMLFontLoader::load_font(const std::wstring& font_name) const
{
	// try to find node
	const pugi::xml_node& node = utils::find_node(root_, L"font", font_name);
	if (!node) return boost::shared_ptr<Font>();
	return font_manager_.get(utils::from_wstr(node.child(L"file").child_value()));
}

void XMLFontLoader::load_all_fonts()
{
	for (pugi::xml_node n = root_.child(L"font"); n; n = n.next_sibling(L"font"))
		font_manager_.load(utils::from_wstr(n.child(L"file").child_value()));
}

}}

#include "gui/oglft_font.hpp"

namespace mhe {
namespace gui {

void oglftFont::set_fg_color(const colorf& c)
{
	fnt->setForegroundColor(c.r(), c.g(), c.b());
}


void oglftFont::set_bg_color(const colorf& c)
{
	fnt->setBackgroundColor(c.r(), c.g(), c.b());
}


void oglftFont::print_impl(cmn::uint x, cmn::uint y, const std::string& text)
{
	fnt->draw(x, y, text.c_str());
}


void oglftFont::print_impl(cmn::uint x, cmn::uint y, const std::wstring& text)
{
	throw mhe::exception("Not implemented");
}


bool oglftFont::load(const std::string& fontname, cmn::uint size)
{
	// create new font first
	fnt.reset(new OGLFT::Monochrome(fontname.c_str(), size));
	if (!fnt->isValid())
	{
		fnt.reset();
		return false;
	}
	return true;
}


iFont* oglftFontManager::load_impl(const std::string& fontname, cmn::uint size)
{
	oglftFont *fnt = new oglftFont;
	if (fnt->load(fontname, size))
		return fnt;
	delete fnt;
	return nullptr;
}

};	// gui
};	// mhe

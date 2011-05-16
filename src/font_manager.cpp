#include "font_manager.hpp"

namespace mhe
{
	FontManager::FontManager() :
		tm_(0)
	{}

	boost::shared_ptr<gui::iFont> FontManager::get(const std::string& name)
	{
		const std::string& s_name = get_file_name(name);
        strptrmap::const_iterator it = fontmap_.find(s_name);
        if (it == fontmap_.end())
        {
            // try to load
            return load_impl(name, s_name);
        }
        return it->second;
	}

	boost::shared_ptr<gui::iFont> FontManager::load_impl(const std::string& name,
														const std::string& sname)
	{
		boost::shared_ptr<gui::iFont> font(load_font(name));

		if (!font)
            utils::global_log::instance().write("Failed to load font " + name);
        else
            utils::global_log::instance().write("Font " + name + " loaded");

		std::string nm = sname;
		if (!font->name().empty())
			nm = font->name();
		fontmap_[nm] = font;
		return font;
	}

	gui::iFont* FontManager::load_font(const std::string& name) const
	{
		const std::string& ext = get_file_extensiton(name);
        if (ext.empty()) return 0;

		if (ext == "fnt")
		{
			gui::BMFont* font = new gui::BMFont;
			if (!font->load(name))
			{
				delete font;
				return 0;
			}
			return font;
		}

		return 0;
	}
}

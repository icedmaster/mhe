#ifndef _FONT_MANAGER_HPP_
#define _FONT_MANAGER_HPP_

#include "gui/bmfont.hpp"
#include "texture_manager.hpp"

namespace mhe
{
	class FontManager
	{
		private:
			TextureManager* tm_;
			
			typedef std::map< std::string, boost::shared_ptr<gui::iFont> > strptrmap;
			strptrmap fontmap_;
			
			boost::shared_ptr<gui::iFont> load_impl(const std::string& name,
													const std::string& sname);
			gui::iFont* load_font(const std::string& name) const;
		public:
			FontManager();
			
			boost::shared_ptr<gui::iFont> get(const std::string& name);
	};
}

#endif

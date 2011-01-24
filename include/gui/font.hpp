#ifndef _FONT_HPP_
#define _FONT_HPP_

#include "ifont.hpp"
#include "impl/system_factory.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	namespace gui
	{
		class iFontManager
		{
			private:
				virtual iFont* load_impl(const std::string&, cmn::uint) = 0;
			public:
				iFont* load(const std::string& fontname, cmn::uint size)
				{
					return load_impl(fontname, size);
				}
		};

		// singleton font manager
		class FontManager
		{
			private:
				boost::shared_ptr<iFontManager> impl_;
				FontManager() :
					impl_(GUIFactory::instance().createFontManager())
				{}
				FontManager(const FontManager&) {}
				~FontManager() {}
				void operator=(const FontManager&) {}
			public:
				static FontManager& instance()
				{
					static FontManager fm;
					return fm;
				}

				iFont* load(const std::string& fontname, cmn::uint size)
				{
					return impl_->load(fontname, size);
				}
		};

	};	// gui

};	// mhe

#endif

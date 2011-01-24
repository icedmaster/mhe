#ifndef _OGLFT_FONT_HPP_
#define _OGLFT_FONT_HPP_

#include "ifont.hpp"
#include "font.hpp"
#include "oglft/OGLFT.h"

namespace mhe
{
	namespace gui
	{
		class oglftFont : public iFont
		{
			private:
				std::string name_;
				cmn::uint size_;
				boost::shared_ptr<OGLFT::Face> fnt;
			private:
				const std::string& get_name() const
				{
					return name_;
				}

				void set_name(const std::string& nm)
				{
					// do nothing
				}

				cmn::uint get_size() const
				{
					return size_;
				}

				void set_size(cmn::uint size)
				{
					// do nothing
				}

				void set_fg_color(const colorf&);
				void set_bg_color(const colorf&);
				void print_impl(cmn::uint, cmn::uint, const std::string&);
				void print_impl(cmn::uint, cmn::uint, const std::wstring&);

			public:
				bool load(const std::string& fontname, cmn::uint size);
		};


		class oglftFontManager : public iFontManager
		{
			private:
				iFont* load_impl(const std::string&, cmn::uint);
		};

	};	// gui

};	// mhe

#endif

#ifndef _GCN_FONT_HPP_
#define _GCN_FONT_HPP_

#include "ifont.hpp"
#include "font.hpp"
#include "guichan/font.hpp"

namespace mhe
{
    namespace gui
    {
        class gcnFont : public iFont
        {
            private:
				std::string name_;
				cmn::uint size_;
				boost::shared_ptr<gcn::ImageFont> fnt;
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
        }
    }
}

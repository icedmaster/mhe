#ifndef _IFONT_HPP_
#define _IFONT_HPP_

#include "types.hpp"
#include "geom.hpp"

namespace mhe
{
	namespace gui
	{
		class iFont
		{
			private:
				virtual const std::string& get_name() const = 0;
				virtual void set_name(const std::string&) = 0;
				virtual cmn::uint get_size() const = 0;
				virtual void set_size(cmn::uint) = 0;
				virtual void set_fg_color(const colorf&) = 0;
				virtual void set_bg_color(const colorf&) = 0;
				virtual void print_impl(cmn::uint, cmn::uint, const std::string&) = 0;
				virtual void print_impl(cmn::uint, cmn::uint, const std::wstring&) = 0;
			public:
				const std::string& name() const
				{
					return get_name();
				}

				void setName(const std::string& name)
				{
					set_name(name);
				}

				cmn::uint size() const
				{
					return get_size();
				}

				void setSize(cmn::uint size)
				{
					set_size(size);
				}

				void setForegroundColor(const colorf& c)
				{
					set_fg_color(c);
				}

				void setBackgroundColor(const colorf& c)
				{
					set_bg_color(c);
				}

				void print(cmn::uint x, cmn::uint y, const std::string& s)
				{
					print_impl(x, y, s);
				}

				void print(cmn::uint x, cmn::uint y, const std::wstring& ws)
				{
					print_impl(x, y, ws);
				}
		};

	};	// gui

};	// mhe

#endif

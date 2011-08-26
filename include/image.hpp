#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include "types.hpp"
#include "mhe_math.hpp"
#include <vector>
#include <string>

namespace mhe
{
	class Image
	{
		protected:
			virtual bool load_impl(const std::string& fn) = 0;
			virtual bool load_impl(std::istream&) = 0;
			virtual bool save_impl(const std::string& fn) = 0;
			virtual bool save_impl(std::ostream&) = 0;
			virtual const std::vector<char>& get_impl() const = 0;
			virtual void set_impl(const std::vector<char>&, cmn::uint, cmn::uint) = 0;
			virtual cmn::uint get_width() const = 0;
			virtual cmn::uint get_height() const = 0;
			virtual cmn::uint get_bpp() const = 0;
		public:
			virtual ~Image()
			{}

			bool load(const std::string& fn)
			{
				return load_impl(fn);
			}

			bool load(std::istream& stream)
			{
			    return load_impl(stream);
			}

			bool save(const std::string& fn)
			{
				return save_impl(fn);
			}

			cmn::uint width() const
			{
				return get_width();
			}

			cmn::uint height() const
			{
				return get_height();
			}

			cmn::uint bpp() const
			{
				return get_bpp();
			}

			const std::vector<char>& get() const
			{
				return get_impl();
			}

			void set(const std::vector<char>& d, cmn::uint w, cmn::uint h)
			{
				set_impl(d, w, h);
			}
	};

	void swapRB(Image& im);
	void swapRB(cmn::uint bpp, const std::vector<char>& in, std::vector<char>& out);
	void addAlphaChannel(Image& im, char a);
	void changeColorToAlpha(Image& im, const colorf& c, char a);
};

#endif

#ifndef _SIWA_IMAGE_HPP_
#define _SIWA_IMAGE_HPP_

#include "bmp_image.hpp"

namespace mhe
{
    class siwa_image : public Image
    {
        private:
            bmp_image bmp;
        private:
            cmn::uint get_width() const
			{
				return bmp.width();
			}

			cmn::uint get_height() const
			{
				return bmp.height();
			}

			cmn::uint get_bpp() const
			{
				return bmp.bpp();
			}

			// Functions, that need to implement
			bool load_impl(const std::string& fn);
			bool load_impl(std::istream& stream);

			// saving are not supported
			bool save_impl(const std::string&/* fn*/)
			{
			    return false;
			}
			bool save_impl(std::ostream&/* stream*/)
			{
			    return false;
			}

			void set_impl(const std::vector<char>& d, cmn::uint w, cmn::uint h)
			{
				bmp.set(d, w, h);
			}

			const std::vector<char>& get_impl() const
			{
				return bmp.get();
			}
    };
}

#endif

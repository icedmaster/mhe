#ifndef _PNG_IMAGE_HPP_
#define _PNG_IMAGE_HPP_

#include "image.hpp"
#include <cstring>	// for memset()

namespace mhe
{
	class png_image : public Image
	{
		private:
			std::vector<char> data;
			struct png_file_info
			{
				cmn::uint width;
				cmn::uint height;
				cmn::uint bpp;
				int alpha;
			};

			png_file_info fi;
		private:
			cmn::uint get_width() const
			{
				return fi.width;
			}

			cmn::uint get_height() const
			{
				return fi.height;
			}

			cmn::uint get_bpp() const
			{
				return fi.bpp;
			}

			// Functions, that need to implement
			bool load_impl(const std::string& fn);
			bool load_impl(std::istream&)
			{
			    return false;
			}
			bool save_impl(const std::string& fn);
			bool save_impl(std::ostream&)
			{
			    return false;
			}

			void set_impl(const std::vector<char>& d, cmn::uint w, cmn::uint h)
			{
				data.assign(d.begin(), d.end());
				fi.width = w;
				fi.height = h;
				fi.bpp = (d.size() / w / h) << 3;
			}

			const std::vector<char>& get_impl() const
			{
				return data;
			}

		public:
			png_image()
			{
				memset((char*)&fi, 0, sizeof(fi));
			}
	};

};

#endif


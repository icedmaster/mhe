#ifndef _BMP_IMAGE_HPP_
#define _BMP_IMAGE_HPP_

#include "image.hpp"
#include <cstring>	// for memset()

namespace mhe
{
	const cmn::uint file_header_size = 14;
	const cmn::uint file_info_size   = 40;

	class bmp_image : public Image
	{
		private:
			struct bmp_file_header
			{
				cmn::uint16_t type;
				cmn::uint32_t size;
				cmn::uint16_t res1;
				cmn::uint16_t res2;
				cmn::uint32_t offs;
			};

			struct bmp_file_info
			{
				cmn::uint32_t size;
				cmn::uint32_t width;
				cmn::uint32_t height;
				cmn::uint16_t planes;
				cmn::uint16_t bit_count;
				cmn::uint32_t compression;
				cmn::uint32_t size_image;
				cmn::uint32_t xppm;
				cmn::uint32_t yppm;
				cmn::uint32_t clr_used;
				cmn::uint32_t clr_important;
			};

			bmp_file_info fi;
			std::vector<char> data;

			bool correct_header(const std::vector<char>& h);

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
				return fi.bit_count;
			}

			// Functions, that need to implement
			bool load_impl(const std::string& fn);
			bool load_impl(std::istream& stream);
			bool save_impl(const std::string& fn);
			bool save_impl(std::ostream& stream);

			void set_impl(const std::vector<char>& d, cmn::uint w, cmn::uint h)
			{
				data.assign(d.begin(), d.end());
				fi.width = w;
				fi.height = h;
				fi.bit_count = (d.size() / w / h) << 3;
			}

			const std::vector<char>& get_impl() const
			{
				return data;
			}

		public:
			bmp_image()
			{
				memset((char*)&fi, 0, file_info_size);
			}
	};

};

#endif


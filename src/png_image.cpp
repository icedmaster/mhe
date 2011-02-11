#include "png_image.hpp"
#include <fstream>

extern "C"
{
	#include <png.h>
	#include <zlib.h>
};

#define __debug__

#ifdef __debug__
	#include <iostream>
#endif

namespace
{
	void mhe_png_read(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		std::istream *stream = static_cast<std::istream*>(png_get_io_ptr(png_ptr));
		stream->read((char*)data,length);
	}

	void mhe_png_error(png_structp png_ptr, png_const_charp error_msg)
	{
		throw mhe::exception(error_msg);
	}

	void mhe_png_warning(png_structp png_ptr, png_const_charp warning_msg)
	{
		// do nothing
	}
};

namespace mhe
{
	bool png_image::load_impl(const std::string& fn)
	{
		std::ifstream f(fn.c_str(), std::ios::binary);
		if (!f.is_open())
			return false;

		png_structp png;
		png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		png_set_error_fn(png, png_get_error_ptr(png), mhe_png_error, mhe_png_warning);

		try
		{
			png_infop info, endinfo;
			info = png_create_info_struct(png);
			endinfo = png_create_info_struct(png);

			// read first 8 bytes - PNG header
			cmn::uchar header[8];
			f.read((char*)header, 8);
			if ( (f.gcount() == 8) && png_check_sig((png_byte*)header, 8) )
			{
				png_set_read_fn(png, &f, mhe_png_read);
			}
			else
			{
				#ifdef __debug__
				std::cout << "Invalid PNG signature\n";
				#endif
				png_destroy_read_struct(&png, &info, &endinfo);
				f.close();
				return false;
			}

			png_set_sig_bytes(png, 8);

			int color, bpp;
			png_uint_32 w, h;
			png_read_info(png, info);
            png_get_IHDR(png, info, &w, &h, &bpp, &color, NULL, NULL, NULL);

			fi.width  = w;
			fi.height = h;
			fi.bpp    = bpp;
			#ifdef __debug__
			std::cout << fn << " w:" << fi.width << " h:" << fi.height << " b:" << fi.bpp << "\n";
			std::cout << "color:" << color << "\n";
			#endif

			if (color == 2)	// haven't alpha channel
				fi.bpp = bpp * 3;
			if (color == 6)
				fi.bpp = bpp * 4;


			if (bpp > 8)
				png_set_swap(png);	// works only with little-endian machines

			// taken from OSG
			if (color == PNG_COLOR_TYPE_PALETTE)
                png_set_palette_to_rgb(png);
            if (color == PNG_COLOR_TYPE_GRAY && bpp < 8)
                png_set_gray_1_2_4_to_8(png);
            if (png_get_valid(png, info, PNG_INFO_tRNS))
                png_set_tRNS_to_alpha(png);

			if (bpp < 8)
				png_set_packing(png);

			png_read_update_info(png, info);

			data.resize(png_get_rowbytes(png, info) * fi.height);
			// init additional vector. It contains every row address
			std::vector<png_bytep> r;
			r.resize(fi.height);

			// init addresses
			for (cmn::uint i = 0; i < fi.height; ++i)
				r[i] = (png_bytep)&data[png_get_rowbytes(png, info) * (fi.height - i - 1)];

			// read image
			png_read_image(png, &r[0]);

			// errors in png_read_end are not fatal
			try
			{
				png_read_end(png, endinfo);
			}
			catch (mhe::exception& e)
			{
				#ifdef __debug__
				std::cout << "png_read_end() thrown: " << e.what() << "\n";
				#endif
			}

			png_destroy_read_struct(&png, &info, &endinfo);
			f.close();
			return true;
		}
		catch (mhe::exception& e)
		{
			#ifdef __debug__
			std::cout << "load_image exception: " << e.what() << std::endl;
			#endif
			f.close();
			return false;
		}
	}
};


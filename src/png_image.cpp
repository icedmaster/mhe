#include "png_image.hpp"
#include "utils/logutils.hpp"
#include <fstream>

extern "C"
{
	#include <png.h>
	#include <zlib.h>
};

namespace
{
	void mhe_png_read(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		std::istream *stream = static_cast<std::istream*>(png_get_io_ptr(png_ptr));
		stream->read((char*)data,length);
	}

	void mhe_png_write(png_structp png_ptr, png_bytep data, png_size_t length)
	{
        std::ostream *stream = static_cast<std::ostream*>(png_get_io_ptr(png_ptr));
        stream->write((char*)data, length);
	}

	void mhe_png_flush(png_structp png_ptr)
    {
        std::ostream *stream = static_cast<std::ostream*>(png_get_io_ptr(png_ptr));
        stream->flush();
    }

	void mhe_png_error(png_structp /*png_ptr*/, png_const_charp error_msg)
	{
		throw mhe::exception(error_msg);
	}

	void mhe_png_warning(png_structp /*png_ptr*/, png_const_charp /*warning_msg*/)
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
				WARN_LOG(fn << "Invalid PNG signature");
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
			DEBUG_LOG(fn << " w:" << fi.width << " h:" << fi.height << " b:" << fi.bpp << " color: " << color);

			if (color == 2)	// haven't alpha channel
				fi.bpp = bpp * 3;
			if (color == 6)
				fi.bpp = bpp * 4;


			if (bpp > 8)
				png_set_swap(png);	// works only with little-endian machines

			// taken from OSG
			if (color == PNG_COLOR_TYPE_PALETTE)
                png_set_palette_to_rgb(png);
            //if (color == PNG_COLOR_TYPE_GRAY && bpp < 8)
            //    png_set_gray_1_2_4_to_8(png);
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
				WARN_LOG("png_read_end() thrown: " << e.what());
			}

			png_destroy_read_struct(&png, &info, &endinfo);
			f.close();
			return true;
		}
		catch (mhe::exception& e)
		{
			WARN_LOG("load_image exception: " << e.what());
			f.close();
			return false;
		}
	}

	bool png_image::save_impl(const std::string& fn)
	{
	    const int compression_level = 5;
	    std::ofstream f(fn.c_str(), std::ios::binary);
		if (!f.is_open())
			return false;

	    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	    if (!png) return false;

	    png_infop info = png_create_info_struct(png);
	    if (!info) return false;

	    png_set_write_fn(png, &f, mhe_png_write, mhe_png_flush);
	    png_set_compression_level(png, compression_level);

	    int color = 2;  // without alpha channel
	    if (fi.bpp == 32)
            color = 6;

        // bytes per pixel
        const cmn::uint bts = fi.bpp >> 3;

        // calculate bits per pixel (now use only 8)
        cmn::uint bpp = 8;

        // write header info
        png_set_IHDR(png, info, fi.width, fi.height,
                     bpp, color, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        // init data for writing
        std::vector<png_bytep> r;
        r.resize(fi.height);
        for (cmn::uint i = 0; i < fi.height; ++i)
            r[i] = (png_bytep)&data[fi.width * (fi.height - i - 1) * bts];

        // write data to file
        png_write_info(png, info);
        png_write_image(png, &r[0]);
        png_write_end(png, 0);

	    png_destroy_write_struct(&png, &info);
	    f.close();
		return true;
	}
};


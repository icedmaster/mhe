#include "res/image_loaders/tga_loader.hpp"

#include <cstring>
#include "core/assert.hpp"
#include "render/image.hpp"

namespace mhe {

namespace detail {

struct TGAHeader
{
	char header[12];
};

struct TGA
{
	char header[6];
};

const char uncompressed_tga_header[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const char uncompressed_tga_header_bw[12] = {0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool load_uncompressed_tga(Image& image, std::istream& stream)
{
	TGA tga;
	stream.read(tga.header, sizeof(tga.header));
    image.width = (tga.header[1] << 8) | tga.header[0];
    image.height = (tga.header[3] << 8) | tga.header[2];
	image.bpp = tga.header[4];
	uint size = image.width * image.height * (image.bpp / 8);
	std::vector<uint8_t> data;
	data.resize(size);
	stream.read(reinterpret_cast<char*>(&data[0]), size);

	swap_bgr_rgb(data, image.bpp / 8);

	if (image.bpp == 24)
	{
		image.data.resize(image.width * image.height * 4);
		image.bpp = 32;
		for (size_t i = 0, j = 0; i < size; i += 3, j += 4)
		{
			image.data[j] = data[i];
			image.data[j + 1] = data[i + 1];
			image.data[j + 2] = data[i + 2];
			image.data[j + 3] = 0xff;
		}
	}
	else image.data = data;
	image.mode = image_rgba;
	return true;
}

}

bool load_tga_image(Image& image, std::istream& file)
{
	detail::TGAHeader header;
	file.read(header.header, sizeof(header.header));
	if (::memcmp(header.header, detail::uncompressed_tga_header, 12) && ::memcmp(header.header, detail::uncompressed_tga_header_bw, 12))
	{
		ASSERT(false, "Only uncompressed TGAs are supported now");
		return false;	
	}
	return detail::load_uncompressed_tga(image, file);
}

}

#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <vector>
#include <algorithm>
#include "core/types.hpp"

namespace mhe {

enum
{
	image_rgba,
	image_bgra
};

struct Image
{
	std::vector<uint8_t> data;
	size_t width;
	size_t height;
	size_t bpp;
	int mode;
	bool has_mips;
};

inline void swap_bgr_rgb(std::vector<uint8_t>& data, uint stride)
{
	for (uint i = 0; i < data.size(); i += stride)
		std::swap(data[i], data[i + 2]);
}

}

#endif

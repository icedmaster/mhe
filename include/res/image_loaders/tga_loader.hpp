#ifndef __TGA_LOADER_HPP__
#define __TGA_LOADER_HPP__

#include <fstream>
#include "core/types.hpp"

namespace mhe {

struct Image;

bool load_tga_image(Image& image, std::istream& file, uint32_t flags = 0);

}

#endif

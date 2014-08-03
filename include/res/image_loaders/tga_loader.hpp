#ifndef __TGA_LOADER_HPP__
#define __TGA_LOADER_HPP__

#include <fstream>

namespace mhe {

struct Image;

bool load_tga_image(Image& image, std::istream& file);

}

#endif

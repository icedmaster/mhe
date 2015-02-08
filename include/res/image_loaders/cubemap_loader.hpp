#ifndef __CUBEMAP_LOADER_HPP__
#define __CUBEMAP_LOADER_HPP__

#include <fstream>
#include "core/string.hpp"

namespace mhe {

struct Image;

bool load_cubemap(Image& image, std::istream& stream, const FilePath& filename);

}

#endif

#ifndef __CUBEMAP_LOADER_HPP__
#define __CUBEMAP_LOADER_HPP__

#include <fstream>
#include <string>

namespace mhe {

struct Image;

bool load_cubemap(Image& image, std::istream& stream, const std::string& filename);

}

#endif

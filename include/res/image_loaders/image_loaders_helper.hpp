#ifndef __IMAGE_LOADERS_HELPER_HPP__
#define __IMAGE_LOADERS_HELPER_HPP__

#include <string>

namespace mhe {

struct Image;

bool load_image_by_extension(Image& image, const std::string& filename);

}

#endif

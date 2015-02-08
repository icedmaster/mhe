#ifndef __IMAGE_LOADERS_HELPER_HPP__
#define __IMAGE_LOADERS_HELPER_HPP__

#include "core/string.hpp"

namespace mhe {

struct Image;

bool load_image_by_extension(Image& image, const FilePath& filename);

}

#endif

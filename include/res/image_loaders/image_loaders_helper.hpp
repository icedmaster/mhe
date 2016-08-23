#ifndef __IMAGE_LOADERS_HELPER_HPP__
#define __IMAGE_LOADERS_HELPER_HPP__

#include "core/string.hpp"
#include "core/types.hpp"

namespace mhe {

struct Image;

bool load_image_by_extension(Image& image, const FilePath& filename, uint32_t flags = 0);

}

#endif

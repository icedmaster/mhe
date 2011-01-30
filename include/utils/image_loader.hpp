#ifndef _IMAGE_LOADER_HPP_
#define _IMAGE_LOADER_HPP_

#include "utils/file_utils.hpp"
#include "image.hpp"
#include "bmp_image.hpp"

namespace mhe
{
    Image* load_image(const std::string& filename);
};

#endif

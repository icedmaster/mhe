#include "res/image_loaders/image_loaders_helper.hpp"

#include <fstream>
#include "render/image.hpp"
#include "res/image_loaders/tga_loader.hpp"
#include "utils/file_utils.hpp"

namespace mhe {

bool load_image_by_extension(Image& image, const FilePath& filename, uint32_t flags)
{
    const FilePath& ext = utils::get_file_extension(filename);

    std::ifstream f(filename.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open()) return false;

    image.has_mips = false;

    bool result = false;
    if (ext == "tga" || ext == "png")
        result = load_tga_image(image, f, flags);
    f.close();
    return result;
}

}

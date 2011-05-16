#include "image_loader.hpp"

namespace mhe
{
    Image* load_image(const std::string& filename)
    {
        const std::string& ext = get_file_extensiton(filename);
        if (ext.empty()) return nullptr;

        Image* im = nullptr;
        if (ext == "bmp")
        {
            im = new bmp_image;
            if (im->load(filename))
                return im;
        }
        else if (ext == "png")
        {
            im = new png_image;
            if (im->load(filename))
                return im;
        }
        else if (ext == "siwa")
        {
            im = new siwa_image;
            if (im->load(filename))
                return im;
        }
        if (im != nullptr)
            delete im;
        return nullptr;
    }
};

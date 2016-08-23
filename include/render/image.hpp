#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <algorithm>
#include "core/types.hpp"
#include "core/vector.hpp"

namespace mhe {

enum
{
    image_rgba,
    image_bgra
};

enum
{
    image_flip_v = 1
};

struct Image
{
    vector<uint8_t> data;
    size_t width;
    size_t height;
    size_t bpp;
    int mode;
    bool has_mips;
};

inline void swap_bgr_rgb(vector<uint8_t>& data, size_t stride)
{
    for (uint i = 0; i < data.size(); i += stride)
        std::swap(data[i], data[i + 2]);
}

inline void flip(vector<uint8_t>& data, size_t width, size_t height, size_t stride)
{
    size_t half_height = height / 2;
    for (size_t h = 0; h < half_height; ++h)
    {
        size_t src = h * width * stride;
        size_t dst = (height - 1 - h) * width * stride;
        for (size_t w = 0; w < width; ++w)
        {
            for (size_t s = 0; s < stride; ++s)
            {
                size_t offset = w * stride + s;
                uint8_t t = data[src + offset];
                data[src + offset] = data[dst + offset];
                data[dst + offset] = t;
            }
        }
    }
}

}

#endif

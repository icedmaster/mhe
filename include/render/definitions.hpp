#ifndef __DEFINITIONS_HPP__
#define __DEFINITIONS_HPP__

#include <cstring>

namespace mhe {

// image format
enum
{
    format_rgba,
    format_bgra,
    format_rgba32f,
    format_d24s8,
    format_d24f,
    format_r32f,
    format_r16f,
    format_rgb,
    format_rg16f,
    format_rgba16f,
    format_rgb32f,
    format_rgb16f,
    format_max
};

static const char* format_str[format_max] = {"rgba", "bgra", "rgba32f", "d24s8", "d24f", "r32f", "r16f", "rgb", "rg16f", "rgba16f", "rgb32f", "rgb16f"};

inline int get_format_by_name(const char* name)
{
    for (int i = 0; i < format_max; ++i)
    {
        if (!strcmp(name, format_str[i]))
            return i;
    }
    ASSERT(0, "Invalid format " << name);
    return format_max;
}

// texture datatype
enum
{
    format_ubyte,
    format_float,
    format_uint,
    format_int,
    format_default,
    format_datatype_max
};

// access
enum
{
    access_readwrite,
    access_readonly,
    access_writeonly,
    access_max
};

// barriers
enum
{
    memory_barrier_storage_buffer = 1,
    memory_barrier_image_fetch = 1 << 1
};

const uint32_t memory_barrier_none = 0;
const uint32_t memory_barrier_all = 0xffffffff;

}

#endif

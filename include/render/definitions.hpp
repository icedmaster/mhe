#ifndef __DEFINITIONS_HPP__
#define __DEFINITIONS_HPP__

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
	format_max
};

static const char* format_str[format_max] = {"rgba", "bgra", "rgba32f", "d24s8", "d24f", "r32f", "r16f", "rgb", "rg16f", "rgba16f", "rgb16f"};

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
	memory_barrier_storage_buffer,
	memory_barrier_max
};

}

#endif

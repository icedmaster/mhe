#ifndef __DEFINITIONS_HPP__
#define __DEFINITIONS_HPP__

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
	format_max
};

static const char* format_str[format_max] = {"rgba", "bgra", "rgba32f", "d24s8", "d24f", "r32f", "r16f", "rgb", "rg16f", "rgba16f"};

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
	format_default,
	format_datatype_max
};

#endif

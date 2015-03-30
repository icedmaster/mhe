#ifndef __LAYOUTS_HPP__
#define __LAYOUTS_HPP__

#include "mhe_math.hpp"
#include "core/compiler.hpp"

namespace mhe {

struct Context;

struct MHE_EXPORT StandartGeometryLayout
{
	struct Vertex
	{
		vec3 pos;
		vec3 nrm;
		vec3 tng;
		vec2 tex;
	};

	static const char* name()
	{
		return "standart_geometry";
	}

	static uint16_t handle;

	static void init(Context& context);
};

struct MHE_EXPORT SkyboxLayout
{
	struct Vertex
	{
		vec4 pos;
	};

	static const char* name()
	{
		return "skybox";
	}

	static uint16_t handle;

	static void init(Context& context);
};

struct MHE_EXPORT FullscreenLayout
{
	struct Vertex
	{
		vec4 pos;
		vec2 tex;
	};

	static const char* name()
	{
		return "fullscreen";
	}

	static uint16_t handle;

	static void init(Context& context);
};

struct MHE_EXPORT DebugLayout
{
    struct Vertex
    {
        vec3 pos;
    };

    static const char* name()
    {
        return "debug";
    }

    static uint16_t handle;

    static void init(Context& context);
};

bool init_standart_layouts(Context& context);

}

#endif

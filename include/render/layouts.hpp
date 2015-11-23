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
		vec4 tng;
		vec2 tex;
	};

	static const char* name()
	{
		return "standart_geometry";
	}

	static const uint16_t handle = 0;

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

	static const uint16_t handle = 1;

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

	static const uint16_t handle = 2;

	static void init(Context& context);
};

struct MHE_EXPORT DebugLayout
{
    struct Vertex
    {
        vec3 pos;
		vec3 nrm;
    };

    static const char* name()
    {
        return "debug";
    }

    static const uint16_t handle = 3;

    static void init(Context& context);
};

struct MHE_EXPORT SkinnedGeometryLayout
{
	struct Vertex
	{
		vec3 pos;
		vec3 nrm;
		vec4 tng;
		vec2 tex;
		uint ids[4];
		float weights[4];
	};

	static const char* name()
	{
		return "skinned_geometry";
	}

	static const uint16_t handle = 4;

	static void init(Context& context);
};

bool init_standart_layouts(Context& context);

}

#endif

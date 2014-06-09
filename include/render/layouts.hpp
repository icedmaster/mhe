#ifndef __LAYOUTS_HPP__
#define __LAYOUTS_HPP__

#include "mhe_math.hpp"
#include "core/compiler.hpp"

namespace mhe {

struct Context;

MHE_EXPORT struct StandartGeometryLayout
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

	void init(Context& context);
};

}

#endif

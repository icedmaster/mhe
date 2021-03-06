#ifndef __RENDER_GLOBALS_HPP__
#define __RENDER_GLOBALS_HPP__

#include "debug/rdbg.hpp"

namespace mhe {

struct RenderGlobals
{
	GlobalVar<bool> gl_map_for_buffer_update;
	GlobalVar<bool> use_vsync;
	GlobalVar<float> texture_anisotropic_level;
	GlobalVar<bool> use_normalmaps;

	RenderGlobals();
};

namespace global {
extern GlobalVar<bool> gl_map_for_buffer_update;
extern GlobalVar<bool> use_vsync;
extern GlobalVar<float> texture_anisotropic_level;
extern GlobalVar<bool> use_normalmaps;
}

void setup(RenderGlobals& render_globals);

}

#endif
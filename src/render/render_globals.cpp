#include "render/render_globals.hpp"

namespace mhe {

namespace global {
GlobalVar<bool> gl_map_for_buffer_update;
GlobalVar<bool> use_vsync;
GlobalVar<float> texture_anisotropic_level;
GlobalVar<bool> use_normalmaps;
}

RenderGlobals::RenderGlobals() :
	gl_map_for_buffer_update("gl_map_for_buffer_update", false),
	use_vsync("use_vsync", true),
	texture_anisotropic_level("render_texture_anisotropic_level", 2.0f),
	use_normalmaps("use_normalmaps", true)
{
}

void setup(RenderGlobals& render_globals)
{
	global::gl_map_for_buffer_update = render_globals.gl_map_for_buffer_update;
	global::use_vsync = render_globals.use_vsync;
	global::texture_anisotropic_level = render_globals.texture_anisotropic_level;
	global::use_normalmaps = render_globals.use_normalmaps;
}

}
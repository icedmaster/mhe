#include "render/render_globals.hpp"

namespace mhe {

GlobalVar<bool> gl_map_for_buffer_update;
GlobalVar<bool> use_vsync;
GlobalVar<float> texture_anisotropic_level;

RenderGlobals::RenderGlobals() :
	gl_map_for_buffer_update("gl_map_for_buffer_update", false),
	use_vsync("use_vsync", true),
	texture_anisotropic_level("render_texture_anisotropic_level", 2.0f)
{
}

void setup(RenderGlobals& render_globals)
{
	gl_map_for_buffer_update = render_globals.gl_map_for_buffer_update;
	use_vsync = render_globals.use_vsync;
	texture_anisotropic_level = render_globals.texture_anisotropic_level;
}

}
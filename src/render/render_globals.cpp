#include "render/render_globals.hpp"

namespace mhe {

GlobalVar<bool> gl_map_for_buffer_update;
GlobalVar<bool> use_vsync;

RenderGlobals::RenderGlobals() :
	gl_map_for_buffer_update("gl_map_for_buffer_update", false),
	use_vsync("use_vsync", true)
{
}

void setup(RenderGlobals& render_globals)
{
	gl_map_for_buffer_update = render_globals.gl_map_for_buffer_update;
	use_vsync = render_globals.use_vsync;
}

}
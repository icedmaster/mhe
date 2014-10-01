#include "debug/light_wrapper.hpp"

#include "core/config.hpp"
#include "game/engine.hpp"
#include "render/light.hpp"

#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif

namespace mhe {

void register_light(game::Engine& engine, size_t id, const std::string& name)
{
#ifdef RDBG_ENABLED
	RDBGProcessor& rdbg_processor = engine.rdbg_processor();
if (!rdbg_processor.type_registrated("light"))
{
	RDBGProcessor::Data data("light");
data.attach("diffuse", Vector4, set_diffuse_color, get_diffuse_color);
rdbg_processor.register_type(data);
}
#endif
}

bool set_diffuse_color(game::Engine& engine, uint32_t id, const colorf& color)
{
	if (!engine.context().light_pool.is_valid(id))
		return false;
	engine.context().light_pool.get(id).shading().diffuse = color;
	return true;
}

bool get_diffuse_color(game::Engine& engine, uint32_t id, colorf& color)
{
	if (!engine.context().light_pool.is_valid(id))
		return false;
	color = engine.context().light_pool.get(id).shading().diffuse;
	return true;
}

}

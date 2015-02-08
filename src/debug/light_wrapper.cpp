#include "debug/light_wrapper.hpp"

#include "core/config.hpp"
#include "game/engine.hpp"
#include "render/instances.hpp"

#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif

namespace mhe {

void register_light_type(RDBGProcessor& rdbg_processor)
{
#ifdef RDBG_ENABLED
	if (!rdbg_processor.type_registrated("light"))
	{
		RDBGProcessor::Data data("light");
		data.attach("diffuse", Vector4, set_diffuse_color, get_diffuse_color);
		data.attach("specular", Vector4, set_specular_color, get_specular_color);
		rdbg_processor.register_type(data);
	}
#endif
}

void register_light(game::Engine& engine, size_t id, const std::string& name)
{
    NOT_IMPLEMENTED(id);
    NOT_IMPLEMENTED(name);
#ifdef RDBG_ENABLED
	register_light_type(engine.rdbg_processor());
#endif
}

bool set_diffuse_color(game::Engine& engine, size_t id, const colorf& color)
{
	if (!engine.scene_context().light_pool.is_valid(id))
		return false;
	engine.scene_context().light_pool.get(id).light.shading().diffuse = color;
	return true;
}

bool get_diffuse_color(game::Engine& engine, size_t id, colorf& color)
{
	if (!engine.scene_context().light_pool.is_valid(id))
		return false;
	color = engine.scene_context().light_pool.get(id).light.shading().diffuse;
	return true;
}

bool set_specular_color(game::Engine& engine, size_t id, const colorf& color)
{
	if (!engine.scene_context().light_pool.is_valid(id))
		return false;
	engine.scene_context().light_pool.get(id).light.shading().specular = color;
	return true;
}

bool get_specular_color(game::Engine& engine, size_t id, colorf& color)
{
	if (!engine.scene_context().light_pool.is_valid(id))
		return false;
	color = engine.scene_context().light_pool.get(id).light.shading().specular;
	return true;
}

}

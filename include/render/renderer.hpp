#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "core/string.hpp"
#include "core/hash.hpp"

namespace mhe {

struct Context;
struct RenderContext;
struct SceneContext;
struct NodeInstance;

class MaterialSystem;

bool init_node(NodeInstance& node, Context& context);
void update_nodes(Context& context, RenderContext& render_context, SceneContext& scene_context);
void sort_draw_calls(const Context& context, RenderContext& render_context);

MHE_EXPORT void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context,
                const string& albedo_texture_name, const string& normalmap_texture_name = string());

MHE_EXPORT bool load_node(NodeInstance& instance, const string& name, hash_type material_system_name, Context& context, SceneContext& scene_context);

template <class MaterialSystem>
bool load_node(NodeInstance& node, const string& name, Context& context, SceneContext& scene_context)
{
	return load_node(node, name, MaterialSystem::name(), context, scene_context);
}

}

#endif

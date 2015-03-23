#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "core/string.hpp"
#include "core/hash.hpp"
#include "core/ref_counter.hpp"

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

class Renderer : public ref_counter
{
public:
	static const uint8_t skybox_material_system_priority = 2;
	static const uint8_t shadowmap_depth_write_material_system_priority = 3;
public:
	Renderer(Context& context) : context_(context) {}
    virtual ~Renderer() {}

	virtual void update(RenderContext& render_context, SceneContext& scene_context);
	virtual void render(RenderContext& render_context, SceneContext& scene_context);

    void set_skybox_material_system(MaterialSystem* material_system);
    void set_shadowmap_depth_write_material_system(MaterialSystem* material_system);

	void flush();
protected:
	Context& context()
	{
		return context_;
	}

	virtual void execute_render(RenderContext& render_context);
private:
    virtual void update_impl(Context& /*context*/, RenderContext& /*render_context*/, SceneContext& /*scene_context*/) {}
	virtual void render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context) = 0;

	Context& context_;
	MaterialSystem* skybox_material_system_;
	MaterialSystem* shadowmap_depth_write_material_system_;
	// To implement
	MaterialSystem* transparent_objects_material_system_;
	MaterialSystem* particles_material_system_;
};

}

#endif

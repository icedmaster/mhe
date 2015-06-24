#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "core/string.hpp"
#include "core/hash.hpp"
#include "core/ref_counter.hpp"
#include "core/fixed_size_vector.hpp"
#include "math/vector4.hpp"
#include "render_context.hpp"

namespace mhe {

struct Context;
struct RenderContext;
struct SceneContext;
struct NodeInstance;
struct MaterialInitializationData;

class MaterialSystem;
class PosteffectMaterialSystemBase;

bool init_node(NodeInstance& node, Context& context);
void update_nodes(Context& context, RenderContext& render_context, SceneContext& scene_context);
void sort_draw_calls(const Context& context, RenderContext& render_context);

MHE_EXPORT void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context,
				const string& albedo_texture_name, const string& normalmap_texture_name = string());
MHE_EXPORT void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context,
	const MaterialInitializationData& material_initialization_data);

MHE_EXPORT bool load_node(NodeInstance& instance, const string& name, hash_type material_system_name, Context& context, SceneContext& scene_context);

template <class MaterialSystem>
bool load_node(NodeInstance& node, const string& name, Context& context, SceneContext& scene_context)
{
	return load_node(node, name, MaterialSystem::name(), context, scene_context);
}

class PosteffectSystem
{
public:
	struct PosteffectNodeDesc
	{
		string name;
		string prev_node;
		string material;
	};

	void add(Context& context, const PosteffectNodeDesc& node_desc);
	void process(Context& context, RenderContext& render_context, SceneContext& scene_context);
private:
	struct PosteffectNode
	{
		string name;
		PosteffectMaterialSystemBase* material_system;
	};

	PosteffectNode* find_node(const string& name) const;

	typedef fixed_size_vector<PosteffectNode, 16> Posteffects;
	Posteffects posteffects_;
};

class Renderer : public ref_counter
{
public:
	static const uint8_t skybox_material_system_priority = 2;
	static const uint8_t shadowmap_depth_write_material_system_priority = 3;
	static const uint8_t debug_material_system_priority = 10;

	enum DebugMode
	{
		renderer_debug_mode_none,
		renderer_debug_mode_main,
		renderer_debug_mode_shadows
	};
public:
	Renderer(Context& context);
	virtual ~Renderer() {}

	bool init();

	virtual void before_update(SceneContext& scene_context);
	virtual void update(SceneContext& scene_context);
	virtual void render(SceneContext& scene_context);

	void set_skybox_material_system(MaterialSystem* material_system);
	void set_shadowmap_depth_write_material_system(MaterialSystem* material_system);
	void set_directional_shadowmap_depth_write_material_system(MaterialSystem* material_system);
	void set_fullscreen_debug_material_system(MaterialSystem* material_system);

	void set_ambient_color(const colorf& color)
	{
		ambient_color_ = color;
	}

	void set_debug_mode(DebugMode mode)
	{
		debug_mode_ = mode;
		debug_mode_changed(mode);
	}

	DebugMode debug_mode() const
	{
		return debug_mode_;
	}

	void flush();

	PosteffectSystem& posteffect_system()
	{
		return posteffect_system_;
	}

	RenderContext& render_context()
	{
		return render_context_;
	}
protected:
	Context& context()
	{
		return context_;
	}

	virtual void execute_render(RenderContext& render_context);
protected:
	virtual void debug_mode_changed(DebugMode mode);
private:
	virtual void update_impl(Context& /*context*/, RenderContext& /*render_context*/, SceneContext& /*scene_context*/) {}
	virtual void render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context) = 0;

	Context& context_;
	MaterialSystem* skybox_material_system_;
	MaterialSystem* shadowmap_depth_write_material_system_;
	MaterialSystem* directional_shadowmap_depth_write_material_system_;
	// To implement
	MaterialSystem* transparent_objects_material_system_;
	MaterialSystem* particles_material_system_;

	MaterialSystem* fullscreen_debug_material_system_;

	colorf ambient_color_;
	DebugMode debug_mode_;

	PosteffectSystem posteffect_system_;

	RenderContext render_context_;
};

class NullRenderer : public Renderer
{
public:
	NullRenderer(Context& context) :
		Renderer(context)
		{}
private:
	void render_impl(Context& /*context*/, RenderContext& /*render_context*/, SceneContext& /*scene_context*/) override
	{}
};

bool init_render(Context& context);

}

#endif

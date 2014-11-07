#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "camera_controller.hpp"
#include "render/scene_context.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/config.hpp"
#include "core/ref_ptr.hpp"

#include "debug/rdbg.hpp"

namespace mhe {

struct RenderContext;
struct Context;

class CameraController;

class MHE_EXPORT Scene
{
public:
	Scene();

	const TransformPool& transform_pool() const
	{
		return scene_context_.transform_pool;
	}

	NodeInstance& create_node() const;
	void update(RenderContext& render_context, Context& context);

	size_t nodes(NodeInstance*& nodes, size_t& offset, size_t material_system) const;
	size_t nodes(NodeInstance*& nodes) const;

	void delete_node(uint16_t id);

	AABBInstance& create_aabb() const;

	void set_camera_controller(CameraController* controller)
	{
		camera_controller_ = controller;
	}

	SceneContext& scene_context()
	{
		return scene_context_;
	}
private:
	void refresh_node_material_link(NodeInstance* nodes);
	void update_light_sources(RenderContext& render_context, Context& context);

	struct MaterialConnector
	{
		size_t offset;
		size_t size;
	};

	SceneContext scene_context_;
	MaterialConnector nodes_per_material_[max_material_systems_number];
	ref_ptr<CameraController> camera_controller_;

	GlobalVar<size_t> global_max_lights_number_;
};

}

#endif

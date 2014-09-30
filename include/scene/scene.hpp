#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "camera_controller.hpp"
#include "render/node.hpp"
#include "core/pool.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/config.hpp"
#include "core/ref_ptr.hpp"

namespace mhe {

struct RenderContext;
struct Context;

class CameraController;

typedef Pool<Transform, 4096, uint16_t> TransformPool;
typedef Pool< Node, 4096, uint16_t, StructTypePolicy<Node, uint16_t> > NodePool;

struct SceneContext
{
	uint32_t time;
	uint32_t delta;
};

class MHE_EXPORT Scene
{
public:
	Scene();

	const TransformPool& transform_pool() const
	{
		return transform_pool_;
	}

	Node& create_node() const;
	void update(RenderContext& render_context, Context& context, const SceneContext& scene_context);

	size_t nodes(Node*& nodes, size_t& offset, size_t material_system) const;
	size_t nodes(Node*& nodes) const;

	void delete_node(uint16_t id);

	void set_camera_controller(CameraController* controller)
	{
		camera_controller_ = controller;
	}
private:
	void refresh_node_material_link(Node* nodes);
	void update_light_sources(RenderContext& render_context, Context& context);

	struct MaterialConnector
	{
		size_t offset;
		size_t size;
	};

	TransformPool transform_pool_;
	NodePool node_pool_;
	MaterialConnector nodes_per_material_[max_material_systems_number];
	ref_ptr<CameraController> camera_controller_;
};

}

#endif

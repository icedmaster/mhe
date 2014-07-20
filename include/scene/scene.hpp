#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "render/node.hpp"
#include "core/pool.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/config.hpp"

namespace mhe {

struct RenderContext;

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
	void update(RenderContext& render_context, const SceneContext& scene_context);

	bool nodes(Node*& nodes, size_t& count, size_t material_system) const;
	bool nodes(Node*& nodes, size_t& count) const;

	void delete_node(uint16_t id);
private:
	TransformPool transform_pool_;
	NodePool node_pool_;
	size_t nodes_per_material_[max_material_systems_number];
};

}

#endif

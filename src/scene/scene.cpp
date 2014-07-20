#include "scene/scene.hpp"

#include <algorithm>

#include "render/render_context.hpp"

namespace mhe {

namespace {

struct SortHelper
{
    bool operator() (const Node& n1, const Node& n2) const
    {
        return n1.material.material_system < n2.material.material_system;
    }
};

}

Scene::Scene()
{
	::memset(nodes_per_material_, max_material_systems_number + 1, sizeof(nodes_per_material_));
}

Node& Scene::create_node() const
{
	uint16_t id = node_pool_.create();
	return node_pool_.get(id);
}

void Scene::update(RenderContext& render_context, const SceneContext& /*scene_context*/)
{
    if (camera_controller_ != nullptr)
    {
        camera_controller_->update(render_context);
        const Camera& camera = camera_controller_->camera();
        camera.get(render_context.view, render_context.proj, render_context.vp);
        render_context.viewpos = camera.position();
    }

	Node* nodes = node_pool_.all_objects();
	std::sort(nodes, nodes + node_pool_.size(), SortHelper());
	node_pool_.update();

	uint8_t prev_material_system = max_material_systems_number + 1;
	for (size_t i = 0; i < node_pool_.size(); ++i)
	{
		uint8_t current_material_system = nodes[i].material.material_system;
		if (current_material_system != prev_material_system)
		{
			nodes_per_material_[current_material_system] = i;
			prev_material_system = current_material_system;
		}
	}
}

bool Scene::nodes(Node*& nodes, size_t& count, size_t material_system) const
{
	ASSERT(material_system < max_material_systems_number, "Invalid material_system index " << material_system);
	size_t start = nodes_per_material_[material_system];
	if (start >= max_material_systems_number) return false;
	size_t total = node_pool_.size();
	count = total;
	for (size_t i = material_system + 1; i < max_material_systems_number; ++i)
	{
		if (nodes_per_material_[i] >= max_material_systems_number) continue;
		count = nodes_per_material_[i] - start;
		break;
	}
	nodes = node_pool_.all_objects() + start;
	return true;
}

bool Scene::nodes(Node*& nodes, size_t& count) const
{
	nodes = node_pool_.all_objects();
	count = node_pool_.size();
	return true;
}

void Scene::delete_node(uint16_t id)
{
	if (!node_pool_.is_valid(id)) return;
	Node& node = node_pool_.get(id);
	if (transform_pool_.is_valid(node.transform))
		transform_pool_.remove(node.transform);
}

}

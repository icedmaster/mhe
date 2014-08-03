#include "scene/scene.hpp"

#include <algorithm>

#include "render/render_context.hpp"
#include "render/context.hpp"
#include "render/material_system.hpp"

namespace mhe {

namespace {

struct SortHelper
{
	SortHelper(const Context& context) :
		context_(context)
		{}
    bool operator() (const Node& n1, const Node& n2) const
    {
			MaterialSystem* ms1 = context_.material_systems.get(n1.material.material_system);
			MaterialSystem* ms2 = context_.material_systems.get(n2.material.material_system);
			if (ms1->priority() == ms2->priority())
        return n1.material.material_system < n2.material.material_system;
			return ms1->priority() < ms2->priority();
    }
	const Context& context_;
};

}

Scene::Scene()
{
	::memset(nodes_per_material_, 0, sizeof(nodes_per_material_));
}

Node& Scene::create_node() const
{
	uint16_t id = node_pool_.create();
	Node& node = node_pool_.get(id);
	node.transform = transform_pool_.create();
	return node;
}

void Scene::update(RenderContext& render_context, const Context& context, const SceneContext& /*scene_context*/)
{
    if (camera_controller_ != nullptr)
    {
        camera_controller_->update(render_context);
        const Camera& camera = camera_controller_->camera();
        camera.get(render_context.view, render_context.proj, render_context.vp);
        render_context.viewpos = camera.position();
				render_context.inv_vp = render_context.vp.inverted();
    }

	Node* nodes = node_pool_.all_objects();
	std::sort(nodes, nodes + node_pool_.size(), SortHelper(context));
	node_pool_.update();

	uint8_t prev_material_system = max_material_systems_number + 1;
	size_t size = 0;
	for (size_t i = 0; i < node_pool_.size(); ++i)
	{
		uint8_t current_material_system = nodes[i].material.material_system;
		++size;
		if (current_material_system != prev_material_system)
		{
			nodes_per_material_[current_material_system].offset = i;
			nodes_per_material_[current_material_system].size = size;
			prev_material_system = current_material_system;
			size = 0;
		}
	}
}

size_t Scene::nodes(Node*& nodes, size_t& offset, size_t material_system) const
{
	ASSERT(material_system < max_material_systems_number, "Invalid material_system index " << material_system);
	offset = nodes_per_material_[material_system].offset;
	nodes = node_pool_.all_objects() + offset;
	return nodes_per_material_[material_system].size;
}

size_t Scene::nodes(Node*& nodes) const
{
	nodes = node_pool_.all_objects();
	return node_pool_.size();
}

void Scene::delete_node(uint16_t id)
{
	if (!node_pool_.is_valid(id)) return;
	Node& node = node_pool_.get(id);
	if (transform_pool_.is_valid(node.transform))
		transform_pool_.remove(node.transform);
}

}

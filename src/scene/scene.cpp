#include "scene/scene.hpp"

#include <algorithm>

#include "render/render_context.hpp"
#include "render/context.hpp"
#include "render/material_system.hpp"

#include "scene/culling.hpp"

namespace mhe {

namespace {

struct SortHelper
{
	SortHelper(const Context& context) :
		context_(context)
		{}
    bool operator() (const NodeInstance& n1, const NodeInstance& n2) const
    {
			MaterialSystem* ms1 = context_.material_systems.get(n1.node.main_pass.material.material_system);
			MaterialSystem* ms2 = context_.material_systems.get(n2.node.main_pass.material.material_system);
			if (ms1->priority() == ms2->priority())
        return n1.node.main_pass.material.material_system < n2.node.main_pass.material.material_system;
			return ms1->priority() < ms2->priority();
    }
	const Context& context_;
};

struct LightSortHelper
{
	bool operator() (const LightInstance& light1, const LightInstance& light2)
	{
		if (!light1.enabled) return false;
		if (!light2.enabled) return true;
		return light1.light.type() < light2.light.type();
	}
};

}

Scene::Scene() :
	global_max_lights_number_("max_lights_number", max_lights_number)
{
	::memset(nodes_per_material_, 0, sizeof(nodes_per_material_));
}

NodeInstance& Scene::create_node() const
{
	NodeInstance::IdType id = scene_context_.node_pool.create();
	NodeInstance& node = scene_context_.node_pool.get(id);
	node.transform_id = scene_context_.transform_pool.create();
	node.aabb_id = scene_context_.aabb_pool.create();
	return node;
}

LightInstance& Scene::create_light() const
{
	LightInstance::IdType id = scene_context_.light_pool.create();
	LightInstance& light = scene_context_.light_pool.get(id);
	light.transform_id = scene_context_.transform_pool.create();
	light.aabb_id = scene_context_.aabb_pool.create();
	return light;
}

AABBInstance& Scene::create_aabb() const
{
	return create_and_get(scene_context_.aabb_pool);
}

void Scene::update(RenderContext& render_context, Context& context)
{
    if (camera_controller_ != nullptr)
    {
        camera_controller_->update(render_context);
        const Camera& camera = camera_controller_->camera();
        camera.get(render_context.view, render_context.proj, render_context.vp);
        render_context.viewpos = camera.position();
		render_context.inv_vp = render_context.vp.inverted();

		frustum_culling(camera.camera_frustum(), scene_context_.aabb_pool.all_objects(), scene_context_.aabb_pool.size());
    }

	NodeInstance* nodes = scene_context_.node_pool.all_objects();
	std::sort(nodes, nodes + scene_context_.node_pool.size(), SortHelper(context));
	scene_context_.node_pool.update();

	refresh_node_material_link(nodes);

	update_light_sources(render_context, context);
}

size_t Scene::nodes(NodeInstance*& nodes, size_t& offset, size_t material_system) const
{
	ASSERT(material_system < max_material_systems_number, "Invalid material_system index " << material_system);
	offset = nodes_per_material_[material_system].offset;
	nodes = scene_context_.node_pool.all_objects() + offset;
	return nodes_per_material_[material_system].size;
}

size_t Scene::nodes(NodeInstance*& nodes) const
{
	nodes = scene_context_.node_pool.all_objects();
	return scene_context_.node_pool.size();
}

void Scene::delete_node(uint16_t id)
{
	if (!scene_context_.node_pool.is_valid(id)) return;
	NodeInstance& node = scene_context_.node_pool.get(id);
	if (scene_context_.transform_pool.is_valid(node.transform_id))
		scene_context_.transform_pool.remove(node.transform_id);
}

void Scene::refresh_node_material_link(NodeInstance* nodes)
{
	uint8_t prev_material_system = max_material_systems_number + 1;
	uint8_t current_material_system = prev_material_system;
	size_t size = 0, begin = 0;
	for (size_t i = 0; i < scene_context_.node_pool.size(); ++i)
	{
		current_material_system = nodes[i].node.main_pass.material.material_system;
		if (prev_material_system > max_material_systems_number)
			prev_material_system = current_material_system;
		if (current_material_system != prev_material_system)
		{
			nodes_per_material_[prev_material_system].offset = begin;
			nodes_per_material_[prev_material_system].size = size;
			prev_material_system = current_material_system;
			size = 0;
			begin = i;
		}
		++size;
	}
	if (current_material_system < max_material_systems_number)
	{
		nodes_per_material_[current_material_system].offset = begin;
		nodes_per_material_[current_material_system].size = size;
	}
}

void Scene::update_light_sources(RenderContext& render_context, Context& context)
{
	LightInstance* lights = scene_context_.light_pool.all_objects();
	std::sort(lights, lights + scene_context_.light_pool.size(), LightSortHelper());
	scene_context_.light_pool.update();
	render_context.lights = scene_context_.light_pool.all_objects();
	size_t size = 0;
	for (size_t i = 0; i < scene_context_.light_pool.size(); ++i, ++size)
	{
		if (!lights[i].enabled)
			break;
	}
    render_context.lights_number = min(size, global_max_lights_number_.value());
}

}

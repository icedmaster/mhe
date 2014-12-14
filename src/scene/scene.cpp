#include "scene/scene.hpp"

#include <algorithm>

#include "render/render_context.hpp"
#include "render/context.hpp"
#include "render/material_system.hpp"

namespace mhe {

namespace {

struct SortHelper
{
    SortHelper(const Context& context, const SceneContext& scene_context) :
        context_(context), scene_context_(scene_context)
		{}
    bool operator() (const NodeInstance& n1, const NodeInstance& n2) const
    {
        const AABBInstance& aabb1 = scene_context_.aabb_pool.get(n1.aabb_id);
        const AABBInstance& aabb2 = scene_context_.aabb_pool.get(n2.aabb_id);
        if (aabb1.visible == aabb2.visible)
        {
            MaterialSystem* ms1 = context_.material_systems.get(n1.node.main_pass.material.material_system);
            MaterialSystem* ms2 = context_.material_systems.get(n2.node.main_pass.material.material_system);
            if (ms1->priority() == ms2->priority())
                return n1.node.main_pass.material.material_system < n2.node.main_pass.material.material_system;
			return ms1->priority() < ms2->priority();
        }
        else return static_cast<size_t>(aabb1.visible) > static_cast<size_t>(aabb2.visible);
    }
	const Context& context_;
    const SceneContext& scene_context_;
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
    visible_aabbs_(0),
    visible_nodes_(0),
	global_max_lights_number_("max_lights_number", max_lights_number),
	use_frustum_culling_("use_frustum_culling", true)
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
    stats_.reset();

    if (camera_controller_ != nullptr)
    {
        camera_controller_->update(render_context);
        const Camera& camera = camera_controller_->camera();
        camera.get(render_context.view, render_context.proj, render_context.vp);
        render_context.viewpos = camera.position();
		render_context.inv_vp = render_context.vp.inverted();

        frustum_culling();
    }

	NodeInstance* nodes = scene_context_.node_pool.all_objects();
    size_t nodes_number = scene_context_.node_pool.size();
    std::sort(nodes, nodes + nodes_number, SortHelper(context, scene_context_));
	scene_context_.node_pool.update();

    size_t visible_nodes = 0;
    nodes = scene_context_.node_pool.all_objects();
    for (size_t i = 0; i < nodes_number; ++i, ++visible_nodes)
    {
        AABBInstance& aabb = scene_context_.aabb_pool.get(nodes[i].aabb_id);
        if (!aabb.visible) break;
    }
    visible_nodes_ = visible_nodes;
    stats_.update_nodes(nodes_number, visible_nodes);

	render_context.nodes = nodes;
	render_context.nodes_number = visible_nodes;

	refresh_node_material_link(nodes);
	render_context.material_nodes = material_nodes_;
	render_context.material_nodes_number = max_material_systems_number;

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
    return visible_nodes_;
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
    for (size_t i = 0; i < visible_nodes_; ++i)
	{
		current_material_system = nodes[i].node.main_pass.material.material_system;
		if (prev_material_system > max_material_systems_number)
			prev_material_system = current_material_system;
		if (current_material_system != prev_material_system)
		{
			nodes_per_material_[prev_material_system].offset = begin;
			nodes_per_material_[prev_material_system].size = size;

			material_nodes_[current_material_system].nodes = nodes + begin;
			material_nodes_[current_material_system].size = size;

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

		material_nodes_[current_material_system].nodes = nodes + begin;
		material_nodes_[current_material_system].size = size;
	}
}

void Scene::update_light_sources(RenderContext& render_context, Context& /*context*/)
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

void Scene::frustum_culling()
{
	size_t visible_aabbs = 0;
	if (use_frustum_culling_.value())
	{
		AABBInstance* aabbs = scene_context_.aabb_pool.all_objects();
		const planef* planes = camera_controller_->camera().camera_frustum().planes();
		const planef abs_planes[6] = {abs(planes[0]), abs(planes[1]), abs(planes[2]),
			                          abs(planes[3]), abs(planes[4]), abs(planes[5])};
		
		for (size_t i = 0; i < scene_context_.aabb_pool.size(); ++i)
		{
			bool visible = is_inside(aabbs[i].aabb, planes, abs_planes);
			aabbs[i].visible = visible;
			visible_aabbs += static_cast<size_t>(visible);
		}
		
	}
	else
	{
		visible_aabbs = scene_context_.aabb_pool.size();
		if (use_frustum_culling_.reset_if_changed())
		{
			AABBInstance* aabbs = scene_context_.aabb_pool.all_objects();
			for (size_t i = 0; i < visible_aabbs; ++i)
				aabbs[i].visible = true;
		}
	}
	visible_aabbs_ = visible_aabbs;
    stats_.update_aabbs(scene_context_.aabb_pool.size(), visible_aabbs);
}

}

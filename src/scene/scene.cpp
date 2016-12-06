#include "scene/scene.hpp"

#include <algorithm>

#include "render/render_context.hpp"
#include "render/context.hpp"
#include "render/material_system.hpp"
#include "render/renderer.hpp"
#include "render/light_instance_methods.hpp"
#include "debug/profiler.hpp"

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
        return static_cast<size_t>(aabb1.visible) > static_cast<size_t>(aabb2.visible);
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
        return light1.dblight.type < light2.dblight.type;
    }
};

size_t check_visibility(bool* visibility, const AABBPool& /*parents*/, const AABBPool& parts, const mat4x4& mat)
{
    frustumf f;
    f.set(mat);
    const planef* planes = f.planes();
    const planef abs_planes[6] = {abs(planes[0]), abs(planes[1]), abs(planes[2]),
        abs(planes[3]), abs(planes[4]), abs(planes[5])};
    AABBInstance* aabbs = parts.all_objects();
    size_t size = parts.size();
    size_t total = 0;
    size_t visible = 0;
    for (size_t i = 0; i < size; ++i)
    {
        bool is_visible = is_inside(aabbs[i].aabb, planes, abs_planes);
        visibility[total++] = is_visible;
        visible += static_cast<size_t>(is_visible);
    }
    return visible;
}

}

Scene::Scene(Context& context) :
    context_(context),
    visible_aabbs_(0),
    visible_nodes_(0),
    global_max_lights_number_("max_lights_number", max_lights_number),
    use_frustum_culling_("use_frustum_culling", true)
{
}

NodeInstance& Scene::create_node() const
{
    NodeInstance::IdType id = scene_context_.node_pool.create();
    NodeInstance& node = scene_context_.node_pool.get(id);
    node.transform_id = scene_context_.transform_pool.create();
    node.aabb_id = scene_context_.aabb_pool.create();

    init_node(node, context_);
    return node;
}

LightInstance& Scene::create_light(LightType type) const
{
    LightInstance::IdType id = scene_context_.light_pool.create();
    LightInstance& light = scene_context_.light_pool.get(id);
    light.transform_id = scene_context_.transform_pool.create();
    light.aabb_id = scene_context_.aabb_pool.create();
    light.dblight.type = type;
    init_light(context_, light);
    return light;
}

AABBInstance& Scene::create_aabb() const
{
    return create_and_get(scene_context_.aabb_pool);
}

void Scene::update(RenderContext& render_context)
{
    stats_.reset();

    if (camera_controller_ != nullptr)
    {
        camera_controller_->update(render_context);
        const Camera& camera = camera_controller_->camera();
        camera.get(render_context.main_camera);

        frustum_culling();
    }

    NodeInstance* nodes = scene_context_.node_pool.all_objects();
    size_t nodes_number = scene_context_.node_pool.size();
    std::sort(nodes, nodes + nodes_number, SortHelper(context_, scene_context_));
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

    update_scene_aabb(render_context);

    render_context.nodes = nodes;
    render_context.nodes_number = visible_nodes;

    update_light_sources(render_context);
}

void Scene::process_requests(RenderContext& render_context)
{
    ProfilerElement pe("scene.process_requests");
    for (size_t i = 0; i < max_views_number; ++i)
    {
        ViewId id = static_cast<ViewId>(i);
        if (!render_context.render_view_requests.is_frustum_culling_request_active(id))
            continue;
        FrustumCullingRequestData& request_data = render_context.render_view_requests.frustum_culling_request_data(id);
        bool* visibility = &request_data.result.visibility[0];
        request_data.result.visible = check_visibility(visibility, scene_context_.aabb_pool, scene_context_.parts_aabb_pool, request_data.request.vp);
    }
}

size_t Scene::nodes(NodeInstance*& nodes, size_t& offset, size_t material_system) const
{
    ASSERT(material_system < max_material_systems_number, "Invalid material_system index " << material_system);
    nodes = scene_context_.node_pool.all_objects() + offset;
    return 0;
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

void Scene::update_light_sources(RenderContext& render_context)
{
    LightInstance* lights = scene_context_.light_pool.all_objects();
    std::sort(lights, lights + scene_context_.light_pool.size(), LightSortHelper());
    scene_context_.light_pool.update();
    render_context.lights = scene_context_.light_pool.all_objects();
    size_t size = 0;
    for (size_t i = 0; i < scene_context_.light_pool.size(); ++i, ++size)
    {
        lights[i].shadow_info = nullptr;
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
        parts_frustum_culling(planes, abs_planes);
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

void Scene::parts_frustum_culling(const planef* planes, const planef* abs_planes)
{
    const AABBPool& parents_pool = scene_context_.aabb_pool;
    AABBInstance* aabbs = scene_context_.parts_aabb_pool.all_objects();
    size_t aabbs_number = scene_context_.parts_aabb_pool.size();
    for (size_t i = 0; i < aabbs_number; ++i)
    {
        if (aabbs[i].parent_id != AABBInstance::invalid_id && !parents_pool.get(aabbs[i].parent_id).visible)
            aabbs[i].visible = false;
        else
            aabbs[i].visible = is_inside(aabbs[i].aabb, planes, abs_planes);
    }

    size_t parts = 0, parts_visible = 0;
    NodeInstance* nodes = scene_context_.node_pool.all_objects();
    size_t nodes_number = scene_context_.node_pool.size();
    const AABBPool& parts_aabb_pool = scene_context_.parts_aabb_pool;
    for (size_t i = 0; i < nodes_number; ++i)
    {
        for (size_t j = 0, size = nodes[i].mesh.instance_parts.size(); j < size; ++j, ++parts)
        {
            if (nodes[i].mesh.instance_parts[j].aabb_id != AABBInstance::invalid_id)
                nodes[i].mesh.instance_parts[j].visible = parts_aabb_pool.get(nodes[i].mesh.instance_parts[j].aabb_id).visible;
            else nodes[i].mesh.instance_parts[j].visible = true;
            parts_visible += static_cast<size_t>(nodes[i].mesh.instance_parts[j].visible);
        }
    }
    stats_.update_parts(parts, parts_visible);
}

void Scene::update_scene_aabb(RenderContext& render_context) const
{
    vec3 aabb_min(9999999.0f, 9999999.0f, 9999999.0f);
    vec3 aabb_max(-9999999.0f, -9999999.0f, -9999999.0f);

    // Check aabbs for lights as well. Probably it'd be a good idea to create the additional AABB pool for lights only
    AABBInstance* aabbs = scene_context_.aabb_pool.all_objects();
    for (size_t i = 0; i < visible_aabbs_; ++i)
    {
        vec3 cur_min, cur_max;
        aabbs[i].aabb.min_max(cur_min, cur_max);
        aabb_min = min(cur_min, aabb_min);
        aabb_max = max(cur_max, aabb_max);
    }
    render_context.aabb = AABBf::from_min_max(aabb_min, aabb_max);
}

}

#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "camera_controller.hpp"
#include "render/scene_context.hpp"
#include "render/render_context.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/config.hpp"
#include "core/ref_ptr.hpp"

#include "debug/rdbg.hpp"

namespace mhe {

struct Context;

class CameraController;

class MHE_EXPORT Scene
{
public:
    class Stats
    {
    public:
        void reset()
        {
            prev_state_ = state_;
            ::memset(&state_, 0, sizeof(state_));
        }

        void update_aabbs(size_t total, size_t visible)
        {
            state_.aabbs = total;
            state_.aabbs_visible = visible;
        }

        void update_nodes(size_t total, size_t visible)
        {
            state_.nodes = total;
            state_.nodes_visible = visible;
        }

        size_t aabbs() const
        {
            return prev_state_.aabbs;
        }

        size_t aabbs_visible() const
        {
            return prev_state_.aabbs_visible;
        }

        size_t nodes() const
        {
            return prev_state_.nodes;
        }

        size_t nodes_visible() const
        {
            return prev_state_.nodes_visible;
        }
    private:
        struct State
        {
            size_t aabbs;
            size_t aabbs_visible;
            size_t nodes;
            size_t nodes_visible;
        };

        State state_;
        State prev_state_;
    };
public:
	Scene();

	const TransformPool& transform_pool() const
	{
		return scene_context_.transform_pool;
	}

	NodeInstance& create_node() const;
	void update(RenderContext& render_context, Context& context);

	LightInstance& create_light() const;

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

    const Stats& stats() const
    {
        return stats_;
    }
private:
	void refresh_node_material_link(NodeInstance* nodes);
	void update_light_sources(RenderContext& render_context, Context& context);

    void frustum_culling();

	struct MaterialConnector
	{
		size_t offset;
		size_t size;
	};

	SceneContext scene_context_;
	MaterialConnector nodes_per_material_[max_material_systems_number];
	// TODO: need to remove MaterialConnector struct
	MaterialNodes material_nodes_[max_material_systems_number];
    Stats stats_;
	ref_ptr<CameraController> camera_controller_;
    size_t visible_aabbs_;
    size_t visible_nodes_;

	GlobalVar<size_t> global_max_lights_number_;
	GlobalVar<bool> use_frustum_culling_;
};

}

#endif

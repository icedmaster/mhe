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

				void update_parts(size_t total, size_t visible)
				{
					state_.parts = total;
					state_.parts_visible = visible;
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

				size_t parts() const
				{
					return prev_state_.parts;
				}

				size_t parts_visible() const
				{
					return prev_state_.parts_visible;
				}
    private:
        struct State
        {
            size_t aabbs;
            size_t aabbs_visible;
            size_t nodes;
            size_t nodes_visible;
						size_t parts;
						size_t parts_visible;
        };

        State state_;
        State prev_state_;
    };
public:
    Scene(Context& context);

	const TransformPool& transform_pool() const
	{
		return scene_context_.transform_pool;
	}

	NodeInstance& create_node() const;
	void update(RenderContext& render_context);
	void process_requests(RenderContext& render_context);

	LightInstance& create_light(int type) const;

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
    void update_light_sources(RenderContext& render_context);

	void frustum_culling();
	void parts_frustum_culling(const planef* planes, const planef* abs_planes);

	void update_scene_aabb(RenderContext& render_context) const;

	struct MaterialConnector
	{
		size_t offset;
		size_t size;
	};

    Context& context_;
	SceneContext scene_context_;
    Stats stats_;
	ref_ptr<CameraController> camera_controller_;
    size_t visible_aabbs_;
    size_t visible_nodes_;

	GlobalVar<size_t> global_max_lights_number_;
	GlobalVar<bool> use_frustum_culling_;
};

}

#endif

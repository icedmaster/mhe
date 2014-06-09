#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "node.hpp"
#include "core/pool.hpp"

namespace mhe {

typedef Pool<Transform, uint16_t, 4096> TransformPool;

struct SceneContext
{
	uint32_t time;
	uint32_t delta;
};

class Scene
{
public:
	const TransformPool& transform_pool() const
	{
		return transform_pool_;
	}

	void update(const SceneContext& scene_context);
private:
	TransformPool transform_pool_;
};

}

#endif

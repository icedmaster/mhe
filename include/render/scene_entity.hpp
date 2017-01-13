#ifndef __SCENE_ENTITY_HPP__
#define __SCENE_ENTITY_HPP__

namespace mhe {

struct Context;
struct SceneContext;
struct RenderContext;

class SceneEntity
{
public:
	virtual ~SceneEntity() {}
	virtual void update(Context& /*context*/, SceneContext& /*scene_context*/, RenderContext& /*render_context*/) {}
	virtual void before_render(Context& /*context*/, SceneContext& /*scene_context*/, RenderContext& /*render_context*/) {}
	virtual void after_render(Context& /*context*/, SceneContext& /*scene_context*/, RenderContext& /*render_context*/) {}
};

}

#endif

#include "cpumeshbaker.hpp"

#include <mhe.hpp>

using namespace mhe;

bool CPUMeshBaker::bake(mhe::game::Engine& engine)
{
	TraceContext trace_context;

	trace_context.engine = &engine;

	trace_context.lights = engine.scene_context().lighs_pool.all_objects();
	trace_context.lights_number = engine.scene_context().light_pool.size();

	trace_context.nodes = engine.scene_context().node_pool.all_objects();
	trace_context.nodes_number = engine.scene_context().node_pool.size();

	trace_context.aabbs = engine.scene_context().aabb_pool.all_objects();
	trace_context.aabbs_number = engine.scene_context().aabb_pool.size();

	return true;
}

void CPUMeshBaker::trace(TraceContext& trace_context)
{

}

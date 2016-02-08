#ifndef __CPUMESHBAKER_HPP
#define __CPUMESHBAKER_HPP

namespace mhe {

struct NodeInstance;
struct AABBInstance;
struct LightInstance;

namespace game {

class Engine;

}}

class CPUMeshBaker
{
	struct TraceContext
	{
		mhe::game::Engine* engine;
		mhe::NodeInstance* nodes;
		size_t nodes_number;
		mhe::LightInstance* lights;
		size_t lights_number;
		mhe::AABBInstance* aabbs;
		size_t aabbs_number;
	};
public:
	bool bake(mhe::game::Engine& engine);
private:
	void trace(TraceContext& trace_context);
};

#endif // __CPUMESHBAKER_HPP

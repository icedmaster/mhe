#ifndef __GPU_PROFILER_HPP__
#define __GPU_PROFILER_HPP__

#include "core/string.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/singleton.hpp"
#include "core/hashmap.hpp"
#include "core/pool.hpp"
#include "core/config.hpp"
#include "render/query.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

class GPUProfiler;

class GPUProfilerNode
{
	POOL_ELEMENT_METHODS(uint16_t);
public:
	GPUProfilerNode();
	~GPUProfilerNode();

	void begin();
	void end();

	uint64_t update() const;
private:
	Query begin_[2];
	Query end_[2];
	mutable int current_;
};

class GPUProfiler
{
public:
	struct Node
	{
		string name;
		GPUProfilerNode* node;
		float interval;
		GPUProfilerNode::IdType parent_id;
	};

	typedef fixed_size_vector<Node, gpu_profiler_queries_number> Nodes;
public:
	GPUProfiler() : current_id_(GPUProfilerNode::invalid_id) {}

	GPUProfilerNode& create(const char* name);

	void update();

	void begin(const GPUProfilerNode& node);
	void end(const GPUProfilerNode& node);

	void clear();

	const Nodes& nodes() const
	{
		return nodes_;
	}
private:
	void add(const char* name, GPUProfilerNode& node);

	fixed_size_vector<Node, gpu_profiler_queries_number> nodes_;
	hashmap<string, size_t> indices_;
	hashmap<GPUProfilerNode::IdType, size_t> id_to_index_map_;
	typedef Pool<GPUProfilerNode, gpu_profiler_queries_number, GPUProfilerNode::IdType> NodesPool;
	NodesPool nodes_pool_;
	GPUProfilerNode::IdType current_id_;
};

class MainGPUProfiler : public GPUProfiler, public Singleton<MainGPUProfiler>
{
	friend class Singleton<MainGPUProfiler>;
private:
	MainGPUProfiler() {}
	~MainGPUProfiler() {}
};

class GPUScopedProfilerNode
{
public:
	GPUScopedProfilerNode(GPUProfilerNode& node, GPUProfiler& profiler) :
		node_(node), profiler_(profiler)
	{
		profiler_.begin(node_);
		node_.begin();
	}

	~GPUScopedProfilerNode()
	{
		node_.end();
		profiler_.end(node_);
	}
private:
	GPUProfilerNode& node_;
	GPUProfiler& profiler_;
};

#define GPU_PROFILE(name) GPUProfilerNode& node = MainGPUProfiler::instance().create(name); GPUScopedProfilerNode __node##__LINE__(node, MainGPUProfiler::instance());

}

#endif

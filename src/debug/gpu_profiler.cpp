#include "debug/gpu_profiler.hpp"

namespace mhe {

GPUProfilerNode::GPUProfilerNode() : 
	current_(0)
{
	begin_[0].init(query_timestamp);
	begin_[1].init(query_timestamp);	
	end_[0].init(query_timestamp);
	end_[1].init(query_timestamp);

	begin_[0].set();
	begin_[1].set();
	end_[0].set();
	end_[1].set();
}

GPUProfilerNode::~GPUProfilerNode()
{
	begin_[0].destroy();
	begin_[1].destroy();
	end_[0].destroy();
	end_[1].destroy();
}

void GPUProfilerNode::begin()
{
	begin_[current_].set();
}

void GPUProfilerNode::end()
{
	end_[current_].set();
}

uint64_t GPUProfilerNode::update() const
{
	int prev = (current_ + 1) % 2;
	uint64_t delta = end_[prev].get<uint64_t>() - begin_[prev].get<uint64_t>();
	current_ = prev;
	return delta;
}

void GPUProfiler::add(const char* name, GPUProfilerNode& node)
{
	Node n;
	n.name = name;
	n.node = &node;
	n.interval = 0.0f;
	nodes_.push_back(n);
}

void GPUProfiler::begin(const GPUProfilerNode& node)
{
	hashmap<GPUProfilerNode::IdType, size_t>::iterator it = id_to_index_map_.find(node.id());
	ASSERT(it != id_to_index_map_.end(), "Invalid profiling node");
	size_t index = it->value;
	nodes_[index].parent_id = current_id_;
	current_id_ = node.id();
}

void GPUProfiler::end(const GPUProfilerNode& node)
{
	hashmap<GPUProfilerNode::IdType, size_t>::iterator it = id_to_index_map_.find(node.id());
	ASSERT(it != id_to_index_map_.end(), "Invalid profiling node");
	size_t index = it->value;
	current_id_ = nodes_[index].parent_id;
}

void GPUProfiler::update()
{
	const float ns2us = 1.0f / 1000.0f;
	for (size_t i = 0, size = nodes_.size(); i < size; ++i)
	{
		nodes_[i].interval = nodes_[i].node->update() * ns2us;
	}
}

void GPUProfiler::clear()
{
	for (size_t i = 0, size = nodes_.size(); i < size; ++i)
	{
		nodes_[i].parent_id = GPUProfilerNode::invalid_id;
	}
	current_id_ = GPUProfilerNode::invalid_id;
}

GPUProfilerNode& GPUProfiler::create(const char* name)
{
	string n(name);
	hashmap<string, size_t>::iterator it = indices_.find(n);
	if (it != indices_.end()) return *(nodes_[it->value].node);
	GPUProfilerNode& new_node = create_and_get(nodes_pool_);
	indices_[n] = nodes_.size();
	id_to_index_map_[new_node.id()] = nodes_.size();
	add(name, new_node);
	return new_node;
}

}

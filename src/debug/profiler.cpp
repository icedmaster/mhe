#include "debug/profiler.hpp"

namespace mhe {

ProfilerElement::ProfilerElement(const char* name, int mode, Profiler& profiler, const char* data) :
	profiler_(profiler), mode_(mode), data_(data)
{
	name_ = name == nullptr ? FUNCTION_NAME : name;
	timer_.start();
	profiler_.start(this);
}

ProfilerElement::~ProfilerElement()
{
	if (!timer_.running()) return;
	result_ = timer_.stop();
	profiler_.finish(this);
}

void ProfilerElement::stop()
{
	result_ = timer_.stop();
	profiler_.finish(this);
}

void Profiler::start(ProfilerElement* element)
{
	bool push_new_data = true;
	size_t index = empty_profiler_point;
	if (element->mode() == ProfilerElement::add)
	{
		for (size_t i = 0; i < data_.size(); ++i)
		{
			if (data_[i].name == element->name())
			{
				push_new_data = false;
				index = i;
				break;
			}
		}
	}

	if (push_new_data)
	{
		Data new_data;
		new_data.name = element->name();
		const char* element_data = element->data();
		if (element_data != nullptr)
			new_data.data = element_data;
		new_data.id = data_.size();
		new_data.parent_id = current_point_;
		data_.push_back(new_data);
		index = new_data.id;
	}

	current_point_ = index;
}

void Profiler::finish(ProfilerElement* element)
{
	size_t index = empty_profiler_point;
	for (size_t i = 0; i < data_.size(); ++i)
	{
		if (data_[i].name == element->name())
		{
			index = i;
			break;
		}
	}

	ASSERT(index != empty_profiler_point, "invalid profiler logic");

	Data& data = data_[index];
	++data.count;
	data.interval += element->result();
	current_point_ = data.parent_id;
}

void Profiler::clear()
{
#ifdef PROFILER_SAVE_LAST_FRAME_DATA
	prepare_output_data(last_frame_data_);
#endif
	data_.clear();
	current_point_ = 0;
}

std::vector<Profiler::Data> Profiler::data() const
{
#ifndef PROFILER_SAVE_LAST_FRAME_DATA
	std::vector<Data> result;
	prepare_output_data(result);
	return result;
#else
	return last_frame_data_;
#endif
}

void Profiler::prepare_output_data(std::vector<Profiler::Data>& data) const
{
	data.resize(data_.size());
	for (size_t i = 0; i < data_.size(); ++i)
	{
		data[i].name = data_[i].name;
		data[i].count = data_[i].count;
		data[i].data = data_[i].data;
		data[i].interval = data_[i].interval / data[i].count;
		data[i].id = data_[i].id;
		data[i].parent_id = data_[i].parent_id;
	}
}

}

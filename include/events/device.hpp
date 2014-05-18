#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <vector>
#include "event.hpp"
#include "render/window_system.hpp"
#include "core/fixed_size_vector.hpp"

namespace mhe {

class Device : public ref_counter
{
protected:
	static const size_t max_events_count = 10;
public:
	typedef fixed_size_vector<event_ptr, max_events_count> events_vector;
public:
	Device(const std::string& name) :
		name_(name)
	{}

	virtual ~Device() {}

	const std::string& name() const
	{
		return name_;
	}

	events_vector check(const WindowSystem& ws);
protected:
	/// Initializing device events vector with events of concrete type
	template <class EventType>
	void init_events_with_type()
	{		
		for (size_t i = 0; i < max_events_count; ++i)
			events_[i] = ref_ptr<EventType>(new EventType);
	}
private:
	virtual void check_impl(events_vector& events, const WindowSystem&) = 0;

	std::string name_;
	events_vector events_;
};

}

#endif

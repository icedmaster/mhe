#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <boost/shared_ptr.hpp>
#include "types.hpp"
#include "event_type.hpp"

namespace mhe {

class Device;

class Event
{
public:
	static const int any_event = 0;

	Event(EventType type, int arg = any_event, int optarg = any_event) :
		device_(nullptr)		
	{
		id_ = Event::create_event_id(type, arg, optarg);
	}

	virtual ~Event() {}

	void setup_event(int arg, int optarg = any_event)
	{
		id_ = Event::create_event_id(type(), arg, optarg);
	}

	void set_device(Device* device)
	{
		device_ = device;
	}

	int id() const
	{
		return id_;
	}

	EventType type() const
	{
		return Event::get_event_type(id_);
	}

	int arg() const
	{
		return Event::get_event_arg(id_);
	}

	int optarg() const
	{
		return Event::get_event_optarg(id_);
	}

	Device* device() const
	{
		return device_;
	}

	static int create_event_id(EventType type, int arg, int optarg)
	{
		return ((type << 24) & 0xff000000) | ((arg << 16) & 0xff0000) | (optarg & 0xffff);
	}

	static EventType get_event_type(int id)
	{
		return static_cast<EventType>((id >> 24) & 0xff);
	}

	static int get_event_arg(int id)
	{
		return (id >> 16) & 0xff;
	}

	static int get_event_optarg(int id)
	{
		return id & 0xffff;
	}
private:
	Device* device_;
	int id_;
};

typedef boost::shared_ptr<Event> event_ptr;

}

#endif

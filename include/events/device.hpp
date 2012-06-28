#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "event.hpp"
#include "window_system.hpp"

namespace mhe {

class Device
{
public:
	Device(const std::string& name) :
		name_(name)
	{}

	virtual ~Device() {}

	const std::string& name() const
	{
		return name_;
	}

	std::vector< boost::shared_ptr<Event> > check(const WindowSystem& ws);
private:
	virtual std::vector< boost::shared_ptr<Event> > check_impl(const WindowSystem&) = 0;

	std::string name_;
};

}

#endif

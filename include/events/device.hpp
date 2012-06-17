#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "event.hpp"

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

	std::vector< boost::shared_ptr<Event> > check();
private:
	virtual std::vector< boost::shared_ptr<Event> > check_impl() = 0;

	std::string name_;
};

}

#endif

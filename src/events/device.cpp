#include "events/device.hpp"

namespace mhe {

std::vector< boost::shared_ptr<Event> > Device::check()
{
	const std::vector< boost::shared_ptr<Event> >& events = check_impl();
	for (size_t i = 0; i < events.size(); ++i)
		events[i]->set_device(this);
	return events;
}

}

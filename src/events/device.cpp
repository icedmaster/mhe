#include "events/device.hpp"

namespace mhe {

Device::events_vector Device::check(const WindowSystem& ws)
{
	events_.clear();
	check_impl(events_, ws);
	for (size_t i = 0; i < events_.size(); ++i)
		events_[i]->set_device(this);
	return events_;
}

}

#include "utils/fps_counter.hpp"

namespace mhe {
namespace utils {

FPSCounter::FPSCounter(boost::shared_ptr<InputSystem> is) :
	frames(0), tick(0), seconds(0), last_second_fps(0), callback(0),
	event_listener_(new FPSEventListener(SystemEventType, TICK, 0,
										this, &FPSCounter::on_tick))
{
	is->add_listener(event_listener_);
}

bool FPSCounter::on_tick(const Event& e)
{
	const SystemEvent& se = dynamic_cast<const SystemEvent&>(e);
	cmn::uint timestamp = se.timestamp();
	++frames;
	if (!tick)
		tick = timestamp + 1000;
	else if (timestamp >= tick)
	{
		++seconds;
		tick = timestamp + 1000;
		if (callback)
			callback(this);
	}

	return true;
}

}}

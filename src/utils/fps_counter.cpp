#include "utils/fps_counter.hpp"

namespace mhe {
namespace utils {

FPSCounter::FPSCounter(InputSystem& is) :
	frames(0), tick(0), seconds(0), last_second_fps(0)
{
	is.addListener(new FPSEventListener(SystemEventType, TICK, 0,
										this, &FPSCounter::on_tick));
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
	}

	return true;
}

}}

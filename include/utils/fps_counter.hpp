#ifndef __FPS_COUNTER_HPP__
#define __FPS_COUNTER_HPP__

#include "event.hpp"
#include <boost/function.hpp>

namespace mhe {
namespace utils {

class FPSCounter
{
public:
	typedef boost::function<void (const FPSCounter*)> Callback;
private:
	typedef PrivateEventListener<FPSCounter> FPSEventListener;
	friend class PrivateEventListener<FPSCounter>;

	cmn::uint frames;
	cmn::uint tick;
	cmn::uint seconds;
	cmn::uint last_second_fps;
	Callback callback;

	bool on_tick(const Event& e);
public:
	FPSCounter(InputSystem& is);

	cmn::uint frames_number() const
	{
		return frames;
	}

	cmn::uint seconds_number() const
	{
		return seconds;
	}

	cmn::uint last_fps() const
	{
		return last_second_fps;
	}

	float average_fps() const
	{
		return static_cast<float>(frames) / seconds;
	}

	void set_callback(Callback cb)
	{
		callback = cb;
	}
};

}}	// namespaces 

#endif	// FPS_COUNTER


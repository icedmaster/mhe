#ifndef __FPS_COUNTER_HPP__
#define __FPS_COUNTER_HPP__

#include "event.hpp"
#include "inode.hpp"
#include "gui/ifont.hpp"
#include "strutils.hpp"
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/weak_ptr.hpp>

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

class GraphicsFPSCounter : public FPSCounter, public iNode
{
private:
	boost::weak_ptr<gui::iFont> font_;
	v2d pos_;
	std::wstring text_;

	class Helper
	{
		GraphicsFPSCounter* object_;
	public:
		Helper(GraphicsFPSCounter* object) : object_(object) {}
		void operator()(const FPSCounter* fps_counter)
		{
			object_->update_text(fps_counter);
		}
	};
	friend class Helper;

	void update_text(const FPSCounter*)
	{
		text_ = boost::lexical_cast<std::wstring>(static_cast<int>(average_fps()));
	}
public:
	GraphicsFPSCounter(InputSystem& is, boost::shared_ptr<gui::iFont> font,
					   const v2d& position) :
		FPSCounter(is), font_(font), pos_(position)
	{
		set_callback(Helper(this));
	}

private:
	void draw_impl(const boost::shared_ptr<iDriver>& driver)
	{
		if (font_.expired()) return;
		font_.lock()->print(driver, text_, pos_);
	}

	int get_priority() const
	{
		return 5;
	}
};

}}	// namespaces 

#endif	// FPS_COUNTER


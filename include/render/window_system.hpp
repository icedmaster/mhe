#ifndef _WINDOW_SYSTEM_HPP_
#define _WINDOW_SYSTEM_HPP_

#include "core/ref_ptr.hpp"
#include "core/unique_ptr.hpp"
#include "mhe_math.hpp"
#include "impl/system_factory.hpp"
#include "view.hpp"

namespace mhe {

class MainLoop : public ref_counter
{
public:
	virtual ~MainLoop() {}
	virtual void run(unsigned int update_interval) = 0;
};

struct WindowContextFormat
{
	uint major_version;
	uint minor_version;
	uint build_version;

	WindowContextFormat() :
		major_version(1), minor_version(0), build_version(0)
	{}
};

class WindowSystemImpl
{
public:
	virtual ~WindowSystemImpl() {}

	virtual bool init(const vector2<int>&, int, bool) = 0;
	virtual bool set_format(const WindowContextFormat& format) = 0;
	virtual void close() = 0;
	virtual void set_caption(const std::string&) = 0;
	virtual void swap_buffers() = 0;
	virtual void show_cursor(bool) = 0;
	virtual void resize(const vector2<int>&) = 0;
	virtual View* view() = 0;
	virtual void set_view(View* /*view*/) {}
	virtual MainLoop* main_loop() const
	{
		return nullptr;
	}
};

class WindowSystem
{
public:
	WindowSystem() :
		impl_(SystemFactory::instance().create_window_system())
	{}

	~WindowSystem()
	{
		impl_->close();
	}

	bool init(int w, int h, int bpp, bool fullscreen = false,
			  const WindowContextFormat& format = WindowContextFormat(),
			  const std::string& caption = "MHE")
	{
		win_dim_.set(w, h);
		bpp_ = bpp;
		fullscreen_ = fullscreen;
		bool res = impl_->init(win_dim_, bpp, fullscreen);
		if (!res) return false;
		res = impl_->set_format(format);
		if (res)
			impl_->set_caption(caption);
		return res;
	}

	void swap_buffers()
	{
		impl_->swap_buffers();
	}

	int width() const
	{
		return win_dim_.x();
	}

	int height() const
	{
		return win_dim_.y();
	}

	bool fullscreen() const
	{
		return fullscreen_;
	}

	int bpp() const
	{
		return bpp_;
	}

	const vector2<int>& screen_size() const
	{
		return win_dim_;
	}

	void show_cursor(bool show)
	{
		impl_->show_cursor(show);
	}

	void resize(int w, int h)
	{
		win_dim_.set(w, h);
		impl_->resize(win_dim_);
	}

	void set_caption(const std::string& caption)
	{
		impl_->set_caption(caption);
	}

	View* view()
	{
		return impl_->view();
	}

    const View* view() const
    {
        return impl_->view();
    }

	void set_view(View* view)
	{
		impl_->set_view(view);
	}

	MainLoop* main_loop() const
	{
		return impl_->main_loop();
	}
private:
	unique_ptr<WindowSystemImpl> impl_;
	vector2<int> win_dim_;
	int bpp_;
	bool fullscreen_;
};

}

#endif

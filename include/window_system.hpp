#ifndef _WINDOW_SYSTEM_HPP_
#define _WINDOW_SYSTEM_HPP_

#include "config.hpp"
#include "mhe_math.hpp"
#include "impl/system_factory.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class iWindowSystem
	{
		public:
			virtual ~iWindowSystem() {}

			virtual bool init(const vector2<int>&, int, bool) = 0;
			virtual void close() = 0;
			virtual void setCaption(const std::string&) = 0;
			virtual void swapBuffers() = 0;
			virtual void showCursor(bool) = 0;
	};

	class WindowSystem
	{
		private:
			boost::shared_ptr<iWindowSystem> impl_;
			vector2<int> win_dim_;
			int bpp_;
			bool fullscreen_;
		public:
			WindowSystem() :
				impl_(SystemFactory::instance().createWindowSystem())
			{}

			~WindowSystem()
			{
				impl_->close();
			}

			bool init(int w, int h, int bpp, bool fullscreen = false,
					  const std::string& caption = "MHE")
			{
				win_dim_.set(w, h);
				bpp_ = bpp;
				fullscreen_ = fullscreen;
				bool res = impl_->init(win_dim_, bpp, fullscreen);
				if (res)
					impl_->setCaption(caption);
				return res;
			}

			void swapBuffers()
			{
				impl_->swapBuffers();
			}

			int width() const
			{
				return win_dim_.x();
			}

			int height() const
			{
				return win_dim_.y();
			}

			void showCursor(bool show)
			{
				impl_->showCursor(show);
			}
	};

}

#endif

#ifndef _RENDER3D_HPP_
#define _RENDER3D_HPP_

#include "irender.hpp"
#include "scene.hpp"
#include "mhe_gl.hpp"
#include "window_system.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace mhe
{
	class Render3D : public iRender
	{
		private:
			boost::shared_ptr<WindowSystem> ws_;
			std::vector< boost::shared_ptr<iRender> > subrenders_;

			std::vector< boost::shared_ptr<Scene> > scenes_;

			void save_attributes();
			void restore_attributes();
		private:
			void save_impl();
			void restore_impl();
			void init_impl();

			void add_scene(Scene* scene);

			void remove_scene()
			{}

			void add_subrender(const boost::shared_ptr<iRender>& sr)
			{
				subrenders_.push_back(sr);
			}

			void clear_impl()
			{
				glClear(GL_COLOR_BUFFER_BIT);
			}

			void draw_impl();
			void render_impl();
			void enable_blend();
			void disable_blend();
			void enable_lighting();
			void disable_lighting();
		public:
			Render3D(const boost::shared_ptr<WindowSystem>& ws) :
				ws_(ws)
			{}
	};
};

#endif

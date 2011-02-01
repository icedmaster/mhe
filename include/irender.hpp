#ifndef _IRENDER_HPP_
#define _IRENDER_HPP_

#include "scene.hpp"
#include "camera.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class iRender
	{
		private:
			virtual void save_impl() = 0;
			virtual void restore_impl() = 0;
			virtual void init_impl() = 0;
			virtual void add_scene(iScene* scene) = 0;
			virtual void remove_scene() = 0;
			virtual void add_subrender(const boost::shared_ptr<iRender>&) = 0;
			virtual void add_camera(const boost::shared_ptr<iCamera>&) {}
			virtual void set_active_camera(const std::string&) {}
			virtual void clear_impl() = 0;
			virtual void draw_impl() = 0;
			virtual void render_impl() = 0;
			virtual void enable_blend() = 0;
			virtual void disable_blend() = 0;
			virtual void enable_lighting() = 0;
			virtual void disable_lighting() = 0;
		protected:
		public:
			void init()
			{
				init_impl();
			}

			void add(iScene* scene)
			{
				add_scene(scene);
			}

			//iScene* get() const
			//{
			//	return get_scene();
			//}
			//

			void remove()
			{
				remove_scene();
			}

			void addSubrender(const boost::shared_ptr<iRender>& sr)
			{
				add_subrender(sr);
			}

			void addCamera(const boost::shared_ptr<iCamera>& cam)
			{
				add_camera(cam);
			}

			void setActiveCamera(const std::string& name)
			{
				set_active_camera(name);
			}

			//
			void clear()
			{
				clear_impl();
			}

			void draw()
			{
				draw_impl();
			}

			void render()
			{
				render_impl();
			}

			void save()
			{
				save_impl();
			}

			void restore()
			{
				restore_impl();
			}

			void enableBlend()
			{
				enable_blend();
			}

			void disableBlend()
			{
				disable_blend();
			}

			void enableLighting()
			{
				enable_lighting();
			}

			void disableLighting()
			{
				disable_lighting();
			}
	};
};

#endif

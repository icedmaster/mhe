#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "irenderable.hpp"
#include "input.hpp"
#include "ilight.hpp"
#include "icamera.hpp"
#include "viewport.hpp"
#include <map>
#include <vector>

namespace mhe
{
	class Scene
	{
		private:
			typedef std::map< cmn::uint, boost::shared_ptr<iRenderable> > nodemap;
			nodemap obj_;

			std::vector< boost::shared_ptr<iCamera> > cameras_;
			boost::shared_ptr<iCamera> active_camera_;

			std::vector< boost::shared_ptr<iLight> > lights_;

			boost::shared_ptr<Viewport> view_;

			class TimerListener : public EventListener
			{
				private:
					Scene* scene;
				public:
					TimerListener(Scene *s, EventType et, int type) :
						EventListener(et, type),
						scene(s)
					{}

					bool handle(const Event& ev)
					{
						return scene->handle(ev);
					}
			};

			friend class TimerListener;
			bool handle(const Event&);
		public:
			Scene()
			{
				// default constructor if we don't need handle events
			}

			Scene(InputSystem& is)
			{
				is.addListener(new TimerListener(this, SystemEventType, TIMER));
			}

			Scene(const boost::shared_ptr<Viewport>& v, InputSystem& is) :
				view_(v)
			{
				is.addListener(new TimerListener(this, SystemEventType, TIMER));
			}

			void add(const boost::shared_ptr<iRenderable>& obj);
			const boost::shared_ptr<iRenderable>& getRootNode() const;
			//const boost::shared_ptr<iRenderable>& get(const std::string& name) const;

			void addCamera(const boost::shared_ptr<iCamera>& cam, bool set_active = false)
			{
				cameras_.push_back(cam);
				if (set_active)
				{
					active_camera_ = cam;
					active_camera_->setProjection(view_->w(), view_->h());
				}
			}

			void setActiveCamera(cmn::uint id);

			void addLight(const boost::shared_ptr<iLight>& l)
			{
				lights_.push_back(l);
			}

			void setViewport(const boost::shared_ptr<Viewport>& v)
			{
				view_ = v;
			}

			const boost::shared_ptr<Viewport>& getViewport() const
			{
				return view_;
			}

			void draw();
	};
};

#endif

#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "node.hpp"
#include "icamera.hpp"
#include <set>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class Scene
	{
		public:
			struct Callback
			{
				virtual ~Callback() {}
				virtual void beforeDraw(const Scene*, boost::shared_ptr<Driver>) {}
				virtual void afterDraw(const Scene*, boost::shared_ptr<Driver>) {}
			};
		private:
			typedef std::multiset< boost::shared_ptr<Node>, sort_node_by_pri > nodeset;
			nodeset nodes_;
			std::map<cmn::uint, boost::shared_ptr<iCamera> > cameras_;
			boost::shared_ptr<iCamera> main_camera;	// main camera
			boost::shared_ptr<Callback> callback_;

			std::vector< boost::shared_ptr<Scene> > subscenes;
		public:
			void add(boost::shared_ptr<Node> node)
			{
				nodes_.insert(node);
			}

			void remove(boost::shared_ptr<Node> node)
			{
				//nodes_.erase(node);
				for (nodeset::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
				{
					if (*it == node)
					{
						nodes_.erase(it);
						return;
					}
				}
			}

			void remove(const std::string& name);

			void addCamera(boost::shared_ptr<iCamera> camera)
			{
				cameras_[camera->get_id()] = camera;
			}

			void setMainCamera(cmn::uint id)
			{
			    std::map<cmn::uint, boost::shared_ptr<iCamera> >::iterator it =
					cameras_.find(id);
				if (it == cameras_.end()) return;
				main_camera = it->second;
			}

			void addSubscene(boost::shared_ptr<Scene> scene)
			{
				subscenes.push_back(scene);
			}

			void update(cmn::uint tick);
			void draw(boost::shared_ptr<Driver> driver);

			void setCallback(Callback* callback)
			{
				callback_.reset(callback);
			}

			void setCallback(boost::shared_ptr<Callback> callback)
			{
				callback_ = callback;
			}

			boost::shared_ptr<Node> get_node(const std::string& name) const;
	};
}

#endif

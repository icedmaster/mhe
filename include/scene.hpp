#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "inode.hpp"
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
				virtual void beforeDraw(const Scene*, boost::shared_ptr<iDriver>) {}
				virtual void afterDraw(const Scene*, boost::shared_ptr<iDriver>) {}
			};
		private:
			typedef std::multiset< boost::shared_ptr<iNode>, sort_node_by_pri > nodeset;
			nodeset nodes_;
			std::map<cmn::uint, boost::shared_ptr<iCamera> > cameras_;
			boost::shared_ptr<iCamera> main_camera;	// main camera
			boost::shared_ptr<Callback> callback_;

			std::vector< boost::shared_ptr<Scene> > subscenes;
		public:
			void add(boost::shared_ptr<iNode> node)
			{
				nodes_.insert(node);
			}

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
			void draw(boost::shared_ptr<iDriver> driver);

			void setCallback(Callback* callback)
			{
				callback_.reset(callback);
			}

			void setCallback(boost::shared_ptr<Callback> callback)
			{
				callback_ = callback;
			}
	};
}

#endif

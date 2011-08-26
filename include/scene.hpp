#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "inode.hpp"
#include "icamera.hpp"
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class Scene;

	class SceneCallback
	{
		public:
			virtual void beforeDraw(Scene* const scene, boost::shared_ptr<iDriver>& driver) {}
			virtual void afterDraw(Scene* const scene, boost::shared_ptr<iDriver>& driver) {}
	};

	class Scene
	{
		private:
			typedef std::multiset< boost::shared_ptr<iNode>, sort_node_by_pri > nodeset;
			nodeset nodes_;
			std::vector< boost::shared_ptr<iCamera> > cameras_;
			boost::shared_ptr<iCamera> m_camera;	// main camera
			boost::shared_ptr<SceneCallback> callback_;
		public:
			void add(const boost::shared_ptr<iNode>& node);
			void addCamera(const boost::shared_ptr<iCamera>& camera);

			void draw(const boost::shared_ptr<iDriver>& driver);

			void setCallback(SceneCallback* callback)
			{
				callback_.reset(callback);
			}
	};
}

#endif

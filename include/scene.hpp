#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "inode.hpp"
#include "icamera.hpp"
#include <set>
#include <vector>
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
			std::vector< boost::shared_ptr<iCamera> > cameras_;
			boost::shared_ptr<iCamera> main_camera;	// main camera
			boost::shared_ptr<Callback> callback_;
		public:
			void add(boost::shared_ptr<iNode> node)
			{
				nodes_.insert(node);
			}

			void addCamera(boost::shared_ptr<iCamera> camera)
			{
				cameras_.push_back(camera);
			}

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

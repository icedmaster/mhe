#include "scene.hpp"

namespace mhe
{
	void Scene::add(const boost::shared_ptr<iRenderable>& obj)
	{
		obj_.insert( nodemap::value_type(obj->id(), obj) );
	}

	const boost::shared_ptr<iRenderable>& Scene::getRootNode() const
	{
		//nodemap::const_iterator it = nodemap.find("root");
		nodemap::const_iterator it = obj_.find(0);
		if (it != obj_.end()) return it->second;
		return obj_.begin()->second;
	}

	/*const boost::shared_ptr<iRenderable>& Scene::get(const std::string& name) const
	{
		nodemap::const_iterator it = nodemap.find(name);
		if (it != nodemap.end()) return it->second;
		throw mhe::exception("Node with name " + name + " not found");
	}*/

	void Scene::draw()
	{
		view_->setup();
		if (active_camera_)
		{
            active_camera_->setProjection(view_->w(), view_->h());
            active_camera_->update();
		}

		for (nodemap::const_iterator it = obj_.begin();
									 it != obj_.end();
									 ++it)
			it->second->draw();
	}

	bool Scene::handle(const Event& e)
	{
		for (nodemap::const_iterator it = obj_.begin();
									 it != obj_.end();
									 ++it)
		{
			//it->second->update(e.getTimer().ticks);
		}
		return true;
	}

	void Scene::setActiveCamera(cmn::uint id)
	{
		for (size_t i = 0; i < cameras_.size(); ++i)
		{
			if (cameras_[i]->id() == id)
			{
				active_camera_ = cameras_[i];
				active_camera_->setProjection(view_->w(), view_->h());
			}
		}
	}
};

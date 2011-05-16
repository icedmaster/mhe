#include "scene.hpp"

namespace mhe
{
	void Scene::add(const boost::shared_ptr<iNode>& node)
	{
		nodes_.insert(node);
	}

	void Scene::addCamera(const boost::shared_ptr<iCamera>& camera)
	{
		cameras_.push_back(camera);
		if (!m_camera)
			m_camera = camera;
	}

	void Scene::draw(const boost::shared_ptr<iDriver>& driver)
	{
		if (callback_)
			callback_->beforeDraw(this, driver);

		if (m_camera)
			m_camera->update(driver);
		// simplify draw all nodes
		for (nodeset::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
		{
			it->draw(driver);
		}

		if (callback_)
			callback_->afterDraw(this, driver);
	}
}	// namespace

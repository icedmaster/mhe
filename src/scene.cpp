#include "scene.hpp"

namespace mhe {

void Scene::draw(boost::shared_ptr<iDriver> driver)
{
	for (size_t i = 0; i < subscenes.size(); ++i)
		subscenes[i]->draw(driver);

	if (main_camera)
		main_camera->update(driver);

	if (callback_)
		callback_->beforeDraw(this, driver);

	for (nodeset::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
		(*it)->draw(driver);

	if (callback_)
		callback_->afterDraw(this, driver);
}

void Scene::update(cmn::uint tick)
{
	for (size_t i = 0; i < subscenes.size(); ++i)
		subscenes[i]->update(tick);
	for (nodeset::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
		(*it)->update(tick);

}

boost::shared_ptr<iNode> Scene::get_node(const std::string& name) const
{
	if (name.empty()) return boost::shared_ptr<iNode>();
	for (size_t i = 0; i < subscenes.size(); ++i)
	{
		boost::shared_ptr<iNode> n = subscenes[i]->get_node(name);
		if (n != nullptr) return n;
	}
	for (nodeset::const_iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if ( (*it)->name() == name ) return *it;
	}
	return boost::shared_ptr<iNode>();		
}

void Scene::remove(const std::string& name)
{
	for (size_t i = 0; i < subscenes.size(); ++i)
		subscenes[i]->remove(name);
	for (nodeset::const_iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if ( (*it)->name() == name )
		{
			nodes_.erase(it);
	        break;
		}
	}
}

}	// namespace mhe

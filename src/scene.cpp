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

}	// namespace mhe

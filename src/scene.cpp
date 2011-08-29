#include "scene.hpp"

namespace mhe {

void Scene::draw(boost::shared_ptr<iDriver> driver)
{
	if (callback_)
		callback_->beforeDraw(this, driver);

	for (nodeset::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
		(*it)->draw(driver);

	if (callback_)
		callback_->afterDraw(this, driver);
}

}	// namespace mhe

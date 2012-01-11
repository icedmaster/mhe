#include "game/aspect_manager.hpp"

namespace mhe {
namespace game {

void AspectManager::add(Aspect* aspect)
{
	aspects_.push_back(aspect_ptr(aspect));
}

void AspectManager::add(boost::shared_ptr<Aspect> aspect)
{
	aspects_.push_back(aspect);
}

void AspectManager::update(cmn::uint tick)
{
	// send tick_event to all aspects
	for (size_t i = 0; i < aspects_.size(); ++i)
		aspects_[i]->update(tick);
}

void AspectManager::remove(const std::string& name)
{
	for (std::vector<aspect_ptr>::iterator it = aspects_.begin();
		 it != aspects_.end(); ++it)
	{
		aspect_ptr aspect = *it;
		if (aspect->name() == name)
		{
			aspects_.erase(it);
			return;
		}
	}
}

void AspectManager::remove(Aspect* aspect)
{
	for (std::vector<aspect_ptr>::iterator it = aspects_.begin();
		 it != aspects_.end(); ++it)
	{
		aspect_ptr as = *it;
		if (as.get() == aspect)
		{
			aspects_.erase(it);
			return;
		}
	}
}

void AspectManager::remove(boost::shared_ptr<Aspect> /*aspect*/)
{
	
}

}}

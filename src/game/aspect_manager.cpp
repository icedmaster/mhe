#include "game/aspect_manager.hpp"

namespace mhe {
namespace game {

void AspectManager::add(Aspect* aspect)
{
	aspects_[aspect->full_name()] = aspect_ptr(aspect);
}

void AspectManager::add(boost::shared_ptr<Aspect> aspect)
{
	aspects_[aspect->full_name()] = aspect;
}

void AspectManager::update(cmn::uint tick)
{
	// send tick_event to all aspects
	for (aspects_map::iterator it = aspects_.begin(); it != aspects_.end(); ++it)
		it->second->update(tick);
}

void AspectManager::remove(const std::string& fullname)
{
	aspects_.erase(fullname);
}

void AspectManager::remove(Aspect* aspect)
{
	for (aspects_map::iterator it = aspects_.begin();
		 it != aspects_.end(); ++it)
	{
		aspect_ptr as = it->second;
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

size_t AspectManager::get_by_name(const std::string& name,
								  std::vector< boost::weak_ptr<Aspect> >& aspects) const
{
	for (aspects_map::const_iterator it = aspects_.begin();
		 it != aspects_.end(); ++it)
	{
		if (it->second->name() == name)
			aspects.push_back(boost::weak_ptr<Aspect>(it->second));
	}
	return aspects.size();
}

size_t AspectManager::get_by_add_name(const std::string& name,
									  std::vector< boost::weak_ptr<Aspect> >& aspects) const
{
	for (aspects_map::const_iterator it = aspects_.begin();
		 it != aspects_.end(); ++it)
	{
		if (it->second->add_name() == name)
			aspects.push_back(boost::weak_ptr<Aspect>(it->second));
	}
	return aspects.size();
}

bool AspectManager::get_by_fullname(const std::string& fullname, boost::weak_ptr<Aspect>& aspect) const
{
	aspects_map::const_iterator it = aspects_.find(fullname);
	if (it == aspects_.end()) return false;
	aspect = boost::weak_ptr<Aspect>(it->second);
	return true;
}

}}

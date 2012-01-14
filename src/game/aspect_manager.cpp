#include "game/aspect_manager.hpp"
#include "utils/logutils.hpp"

namespace mhe {
namespace game {

void AspectManager::add(Aspect* aspect)
{
	DEBUG_LOG("AspectManager: add aspect: " << aspect->full_name());
	aspects_[aspect->full_name()] = aspect_ptr(aspect);
}

void AspectManager::add(boost::shared_ptr<Aspect> aspect)
{
	DEBUG_LOG("AspectManager: add aspect: " << aspect->full_name());
	aspects_[aspect->full_name()] = aspect;
}

void AspectManager::update(cmn::uint tick)
{
	// send tick_event to all aspects
	for (aspects_map::iterator it = aspects_.begin(); it != aspects_.end();)
	{
		if (check_aspect_lifetime(tick, it->second))
		{
			it->second->update(tick);
			++it;
		}
		else
		{
			DEBUG_LOG("Destroy aspect: " << it->second->full_name());
			destroy_aspect(it->second.get());
			aspects_.erase(it++);
		}
	}
}

bool AspectManager::check_aspect_lifetime(cmn::uint tick, boost::shared_ptr<Aspect> aspect) const
{
	if (aspect->lifetime())
	{
		if (tick > (aspect->start_time() + aspect->lifetime()))
		{
			DEBUG_LOG("AspectManager: aspect lifetime finished: " << aspect->full_name());
			return false;
		}
	}
	return true;
}

void AspectManager::remove(const std::string& fullname)
{
	aspects_map::iterator it = aspects_.find(fullname);
	if (it == aspects_.end()) return;
	destroy_aspect(it->second.get());
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
			destroy_aspect(aspect);
			aspects_.erase(it);
			return;
		}
	}
}

void AspectManager::remove(boost::shared_ptr<Aspect> aspect)
{
	remove(aspect.get());
}

void AspectManager::destroy_aspect(Aspect* aspect)
{
	DEBUG_LOG("AspectManager: remove aspect: " << aspect->full_name());
	const std::vector< boost::weak_ptr<Aspect> >& childs = aspect->childs();
	for (size_t i = 0; i < childs.size(); ++i)
		aspects_.erase(childs[i].lock()->full_name());
	aspect->update(destroy_event, 0);
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

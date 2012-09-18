#include "scene.hpp"

namespace mhe {

void Scene::draw(const Context& context)
{
	for (nodelist::iterator it = nodes_.begin(); it != nodes_.end();)
	{
		Node* node = (*it).get();
		if (!node->is_visible())
		{
			++it; continue;
		}
		if (!node->is_alive())
		{
			it = nodes_.erase(it);
			continue;
		}
		context.driver()->draw(node);
		++it;
	}
}

void Scene::update(cmn::uint tick)
{
	for (size_t i = 0; i < subscenes_.size(); ++i)
		subscenes_[i]->update(tick);
		
	for (nodelist::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if ((*it)->is_frozen()) continue;
		(*it)->update(tick);	   			
	}

	apply_visitors();
	apply_scene_modifiers();
}

boost::shared_ptr<Node> Scene::get_node(const std::string& name) const
{
	if (name.empty()) return nodeptr();
	for (size_t i = 0; i < subscenes_.size(); ++i)
	{
		boost::shared_ptr<Node> n = subscenes_[i]->get_node(name);
		if (n != nullptr) return n;
	}

	for (nodelist::const_iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if ( (*it)->name() == name ) return *it;
	}
	return nodeptr();		
}

void Scene::add(const nodeptr& node)
{
	nodes_.push_back(node);
}

void Scene::remove(const nodeptr& node)
{
	for (nodelist::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if (*it == node)
		{
			nodes_.erase(it);
			return;
		}
	}
}

void Scene::remove(const std::string& name)
{
	for (size_t i = 0; i < subscenes_.size(); ++i)
		subscenes_[i]->remove(name);

	for (nodelist::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if ((*it)->name() == name)
		{
			nodes_.erase(it);
			return;
		}
	}
}

void Scene::remove_visitor(const std::string& name)
{
	for (std::vector< boost::shared_ptr<NodeVisitor> >::iterator it = visitors_.begin();
		 it != visitors_.end(); ++it)
	{
		if ((*it)->name() == name)
		{
			visitors_.erase(it);
			return;
		}
	}
}

void Scene::remove_modifier(const std::string& name)
{
	for (std::vector< boost::shared_ptr<SceneModifier> >::iterator it = modifiers_.begin();
		 it != modifiers_.end(); ++it)
	{
		if ((*it)->name() == name)
		{
			modifiers_.erase(it);
			return;
		}
	}
}

void Scene::apply_visitors()
{
	for (size_t i = 0; i < visitors_.size(); ++i)
	{
		NodeVisitor* visitor = visitors_[i].get();
		visitor->begin_traverse();
		for (nodelist::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
			(*it)->apply(visitor);
		visitor->end_traverse();
	}
}

void Scene::apply_scene_modifiers()
{
	for (size_t i = 0; i < modifiers_.size(); ++i)
		modifiers_[i]->apply(nodes_);
}

}	// namespace mhe

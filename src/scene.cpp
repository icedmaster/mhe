#include "scene.hpp"

namespace mhe {

Scene::Scene() :
	nodes_(Node::priority_high)
{}

void Scene::draw(const Context& context)
{
	for (size_t i = 0; i < subscenes.size(); ++i)
		subscenes[i]->draw(context);

	if (main_camera)
		main_camera->update(context.driver());

	if (callback_)
		callback_->beforeDraw(this, context.driver());

	for (size_t i = 0; i < nodes_.size(); ++i)
	{
		for (nodelist::iterator it = nodes_[i].begin(); it != nodes_[i].end(); ++it)
			(*it)->draw(context);
	}

	if (callback_)
		callback_->afterDraw(this, context.driver());
}

void Scene::update(cmn::uint tick)
{
	for (size_t i = 0; i < subscenes.size(); ++i)
		subscenes[i]->update(tick);
	for (size_t i = 0; i < nodes_.size(); ++i)
	{
		for (nodelist::iterator it = nodes_[i].begin(); it != nodes_[i].end(); ++it)
			(*it)->update(tick);
	}

}

boost::shared_ptr<Node> Scene::get_node(const std::string& name) const
{
	if (name.empty()) return nodeptr();
	for (size_t i = 0; i < subscenes.size(); ++i)
	{
		boost::shared_ptr<Node> n = subscenes[i]->get_node(name);
		if (n != nullptr) return n;
	}
	for (size_t i = 0; i < nodes_.size(); ++i)
	{
		for (nodelist::const_iterator it = nodes_[i].begin(); it != nodes_[i].end(); ++it)
		{
			if ( (*it)->name() == name ) return *it;
		}
	}
	return nodeptr();		
}

void Scene::add(const nodeptr& node)
{
	nodes_[node->priority()].push_back(node);
}

void Scene::remove(const nodeptr& node)
{
	int pri = node->priority();
	for (nodelist::iterator it = nodes_[pri].begin(); it != nodes_[pri].end(); ++it)
	{
		if (*it == node)
		{
			nodes_[pri].erase(it);
			return;
		}
	}
}

void Scene::remove(const std::string& name)
{
	for (size_t i = 0; i < subscenes.size(); ++i)
		subscenes[i]->remove(name);
	for (size_t i = 0; i < nodes_.size(); ++i)
	{
		for (nodelist::iterator it = nodes_[i].begin(); it != nodes_[i].end(); ++it)
		{
			if ((*it)->name() == name)
			{
				nodes_[i].erase(it);
				return;
			}
		}
	}
}

void Scene::set_node_priority(const nodeptr& node, int priority)
{
	if (node->priority() == priority) return;
	remove(node);
	node->set_priority(priority);
	add(node);
}

}	// namespace mhe

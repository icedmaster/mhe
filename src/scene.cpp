#include "scene.hpp"

namespace mhe {

Scene::Scene() :
	parent_(nullptr)
{}

void Scene::remove_subscene(const Scene* scene)
{
	for (std::vector< boost::shared_ptr<Scene> >::iterator it = subscenes_.begin();
		 it != subscenes_.end(); ++it)
	{
		const boost::shared_ptr<Scene>& s = *it;
		if (s.get() == scene)
		{
			subscenes_.erase(it);
			return;
		}
	}
}

void Scene::draw(Context& context)
{
	if (active_camera_ == nullptr) return;

	context.driver().begin_render();
	active_camera_->update(context.driver());

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
		node->draw(context);
		++it;
	}
	context.driver().end_render();

	for (size_t i = 0; i < subscenes_.size(); ++i)
		subscenes_[i]->draw(context);
}

void Scene::update(cmn::uint tick)
{		
	for (nodelist::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if ((*it)->is_frozen()) continue;
		(*it)->update(tick);	   			
	}

	apply_visitors();
	apply_scene_modifiers(SceneModifier::frame_update);

	for (size_t i = 0; i < subscenes_.size(); ++i)
		subscenes_[i]->update(tick);
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
	apply_scene_modifiers(SceneModifier::node_add);
}

void Scene::remove(const nodeptr& node)
{
	for (nodelist::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
	{
		if (*it == node)
		{
			nodes_.erase(it);
			apply_scene_modifiers(SceneModifier::node_remove);
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

void Scene::add_camera(Camera* camera, bool set_active)
{
	boost::shared_ptr<Camera> new_camera(camera);
	cameras_[camera->name()] = new_camera;
	if (set_active)
		active_camera_ = new_camera;
}

void Scene::remove_camera(const std::string& name)
{
	cameras_.erase(name);
}

void Scene::set_active_camera(const std::string& name)
{
	std::map< std::string, boost::shared_ptr<Camera> >::iterator it = cameras_.find(name);
	if (it == cameras_.end()) return;
	active_camera_ = it->second;
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

void Scene::apply_scene_modifiers(SceneModifier::UpdateMode mode)
{
	for (size_t i = 0; i < modifiers_.size(); ++i)
	{
		if (modifiers_[i]->update_mode() == mode)
			modifiers_[i]->apply(nodes_);
	}
}

}	// namespace mhe

#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "node.hpp"
#include "scene_modifier.hpp"
#include "camera.hpp"
#include <list>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace mhe {

class Scene
{
public:
	Scene();

	void add(Node* node)
	{
		add(nodeptr(node));
	}

	void add(const nodeptr& node);
	void remove(const nodeptr& node);
	void remove(const std::string& name);
	boost::shared_ptr<Node> get_node(const std::string& name) const;

	void add_subscene(const boost::shared_ptr<Scene>& scene)
	{
		subscenes_.push_back(scene);
		scene->parent_ = this;
	}

	void remove_subscene(const Scene* scene);

	void update(cmn::uint tick);
	void draw(const Context& context);

	void add_visitor(NodeVisitor* visitor)
	{
		visitors_.push_back(boost::shared_ptr<NodeVisitor>(visitor));
	}

	void remove_visitor(const std::string& name);

	void add_modifier(SceneModifier* modifier)
	{
		modifiers_.push_back(boost::shared_ptr<SceneModifier>(modifier));
	}

	void remove_modifier(const std::string& name);

	void add_camera(Camera* camera, bool set_active = false);
	void remove_camera(const std::string& name);
	void set_active_camera(const std::string& name);
	Camera* active_camera() const
	{
		return active_camera_.get();
	}

	Scene* parent() const
	{
		return parent_;
	}
private:
	void apply_visitors();
	void apply_scene_modifiers(SceneModifier::UpdateMode mode);

	typedef std::list<nodeptr> nodelist;
	nodelist nodes_;
	Scene* parent_;
	std::vector< boost::shared_ptr<Scene> > subscenes_;
    std::vector< boost::shared_ptr<NodeVisitor> > visitors_;
	std::vector< boost::shared_ptr<SceneModifier> > modifiers_;
	std::map<std::string, boost::shared_ptr<Camera> > cameras_;
	boost::shared_ptr<Camera> active_camera_;
};

}

#endif

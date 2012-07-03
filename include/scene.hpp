#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "node.hpp"
#include "icamera.hpp"
#include <list>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace mhe {

class Scene
{
public:
	struct Callback
	{
		virtual ~Callback() {}
		virtual void beforeDraw(const Scene*, boost::shared_ptr<Driver>) {}
		virtual void afterDraw(const Scene*, boost::shared_ptr<Driver>) {}
	};
public:
	Scene();

	void add(const nodeptr& node);
	void remove(const nodeptr& node);
	void remove(const std::string& name);
	void set_node_priority(const nodeptr& node, int priority);

	void add_camera(boost::shared_ptr<iCamera> camera)
	{
		cameras_[camera->get_id()] = camera;
	}

	void setMainCamera(cmn::uint id)
	{
		std::map<cmn::uint, boost::shared_ptr<iCamera> >::iterator it =
			cameras_.find(id);
		if (it == cameras_.end()) return;
		main_camera = it->second;
	}

	void add_subscene(const boost::shared_ptr<Scene>& scene)
	{
		subscenes.push_back(scene);
	}

	void update(cmn::uint tick);
	void draw(const Context& context);

	void set_callback(Callback* callback)
	{
		callback_.reset(callback);
	}

	void set_callback(boost::shared_ptr<Callback> callback)
	{
		callback_ = callback;
	}

	boost::shared_ptr<Node> get_node(const std::string& name) const;
private:
	typedef std::list<nodeptr> nodelist;
	std::vector<nodelist> nodes_;
	std::map<cmn::uint, boost::shared_ptr<iCamera> > cameras_;
	boost::shared_ptr<iCamera> main_camera;	// main camera
	boost::shared_ptr<Callback> callback_;

	std::vector< boost::shared_ptr<Scene> > subscenes;
};

}

#endif

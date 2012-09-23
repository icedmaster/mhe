#ifndef __SCENE_MODIFIER_HPP__
#define __SCENE_MODIFIER_HPP__

#include <list>
#include <boost/shared_ptr.hpp>

namespace mhe {

class Node;

class SceneModifier
{
public:
	enum UpdateMode
	{
		frame_update,
		node_add,
		node_remove
	};
public:
	SceneModifier(const std::string& name) : 
		name_(name)
	{}

	const std::string& name() const
	{
		return name_;
	}

	virtual ~SceneModifier() {}
	virtual void apply(std::list< boost::shared_ptr<Node> >& nodes) = 0;
	virtual UpdateMode update_mode() const = 0;
private:
	std::string name_;
};

}

#endif

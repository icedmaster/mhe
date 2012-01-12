#ifndef __GAME_STONE_HPP__
#define __GAME_STONE_HPP__

#include "mhe.hpp"

struct StoneParameters
{
	std::string name;
	std::string sprite_name;
	mhe::rect<int> pos;
};

boost::shared_ptr<mhe::game::Aspect> create_stone(mhe::game::mhe_loader& loader, const StoneParameters& sp);

class GameStoneAspect : public mhe::game::NodeAspect
{
public:
	GameStoneAspect(const std::string& fullname,
					boost::shared_ptr<mhe::iNode> node) :
		mhe::game::NodeAspect(fullname, node)
	{}

	GameStoneAspect(const std::string& name, const std::string& add_name,
					boost::shared_ptr<mhe::iNode> node) :
		mhe::game::NodeAspect(name, add_name, node)
	{}
private:
};

#endif

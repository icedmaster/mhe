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

#endif

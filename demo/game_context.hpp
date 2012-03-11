#ifndef __GAME_CONTEXT_HPP__
#define __GAME_CONTEXT_HPP__

#include "mhe.hpp"
#include "game_stone.hpp"

inline int stone_index(int x, int y)
{
	return (x << 8) | y;
}

struct GameContext
{	
	mhe::rect<int> coord;
	std::vector< std::vector<int> > stones;
	int stone_size;
	mhe::vector2<int> prev_selected;
	mhe::vector2<int> prev_clicked;
	std::map< int, boost::weak_ptr<mhe::game::Aspect> > stone_aspects;
	// movement
	cmn::uint stone_moves;
	cmn::uint upd_time;
	// stone creation
	boost::shared_ptr<StoneFactory> stone_factory;
	// effects creation
	boost::shared_ptr<StoneEffectFactory> effect_factory;
	//
	mhe::game::Engine* engine;
};

mhe::v3d get_global_pos(const GameContext& context, const mhe::vector2<int>& pos);
mhe::vector2<int> get_local_pos(const GameContext& context, const mhe::v3d& pos);
boost::shared_ptr<mhe::game::Aspect> get_aspect(const GameContext& context, const mhe::vector2<int>& pos);

#endif

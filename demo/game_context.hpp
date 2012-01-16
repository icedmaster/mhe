#ifndef __GAME_CONTEXT_HPP__
#define __GAME_CONTEXT_HPP__

#include "mhe.hpp"

struct GameContext
{	
	mhe::rect<int> coord;
	std::vector< std::vector<int> > stones;
	int stone_size;
	mhe::vector2<int> selected;
	mhe::vector2<int> prev_selected;
	mhe::vector2<int> clicked;
	mhe::vector2<int> prev_clicked;
	std::map< int, boost::weak_ptr<mhe::game::Aspect> > stone_aspects;
	mhe::game::AspectManager* aspect_manager;
	// movement
	cmn::uint stone_moves;
	cmn::uint upd_time;
};

mhe::v3d get_global_pos(const GameContext& context, const mhe::vector2<int>& pos);
mhe::vector2<int> get_local_pos(const GameContext& context, const mhe::v3d& pos);

#endif

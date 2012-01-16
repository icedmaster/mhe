#include "game_context.hpp"

mhe::v3d get_global_pos(const GameContext& context, const mhe::vector2<int>& pos)
{
	return mhe::v3d(pos.x() * context.stone_size + context.coord.ll().x(),
					pos.y() * context.stone_size + context.coord.ll().y(), 
					0);
}

mhe::vector2<int> get_local_pos(const GameContext& context, const mhe::v3d& pos)
{
	int x = (pos.x() - context.coord.ll().x()) / context.stone_size;
	int y = (pos.y() - context.coord.ll().y()) / context.stone_size;
	return mhe::vector2<int>(x, y);
}

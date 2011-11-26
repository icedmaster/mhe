#ifndef __FIELD_LOGIC_HPP__
#define __FIELD_LOGIC_HPP__

#include "field.hpp"

namespace
{

inline bool is_neighbor(const mhe::vector2<int>& pos1, const mhe::vector2<int>& pos2)
{
	cmn::uint dx = abs(pos1.x() - pos2.x());
	cmn::uint dy = abs(pos1.y() - pos2.y());
	//std::cout << "nn:" << dx << " " << dy << std::endl;
	if ( ((dx == 1) && !dy) || (!dx && (dy == 1)) )
		return true;
	return false;
}

inline bool check_match3(const std::vector< std::vector<int> >& stones, std::vector< mhe::vector2<int> >& del)
{
	const int row_length = 3;
	del.reserve(row_length);

	// horizontal
	for (size_t i = 0; i < stones.size(); ++i)
	{
		int cur = -1, total = 0;
		for (size_t j = 0; j < stones[i].size(); ++j)
		{
			if (!total)
			{
				cur = stones[i][j];
				++total;
			}
			else	
			{
				if (stones[i][j] == cur)
				{
					if (++total == row_length)
					{
						for (int k = 0; k < row_length; ++k)
							del.push_back(mhe::vector2<int>(j - row_length + k + 1, i));
						return true;
					}
				}
				else
				{
					total = 1;
					cur = stones[i][j];
				}
			}
		}
	}

	// vertical
	for (size_t i = 0; i < stones.size(); ++i)
	{
		for (size_t j = 0; j < stones[i].size(); ++j)
		{
			
		}
	}	
	return false;
}

}

class FieldLogicAspect : public mhe::game::Aspect
{
public:
	FieldLogicAspect(const std::string& name,
					 boost::shared_ptr<GameContext> context) :
		mhe::game::Aspect(name), context_(context)
	{}
private:
	void do_subscribe(Aspect* parent)
	{
		parent->subscribe(mhe::game::mouse_click_event, this);
	}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{
		std::cout << context_->clicked.x() << " " << context_->clicked.y() << std::endl;
		mhe::v3d pos(context_->geom.ll().x() + context_->clicked.x() * context_->stone_size,
					 context_->geom.ll().y() + context_->clicked.y() * context_->stone_size, 0);
		mhe::v3d prev_pos(context_->geom.ll().x() + context_->previous_clicked.x() * context_->stone_size,
						  context_->geom.ll().y() + context_->previous_clicked.y() * context_->stone_size, 0);
		if (context_->previous_clicked.x() < 0)
			update_childs(mhe::game::enable_event, &pos);
		else
			do_logic(pos, prev_pos);
		return true;
	}

	void do_logic(const mhe::v3d& pos, const mhe::v3d& prev_pos)
	{
		if (!is_neighbor(context_->previous_clicked, context_->clicked))
		{
			update_childs(mhe::game::disable_event, &prev_pos);
			update_childs(mhe::game::enable_event, &pos);
		}
		else
		{
			int prev_stone = context_->stones[context_->clicked.y()][context_->clicked.x()];
			context_->stones[context_->clicked.y()][context_->clicked.x()] =
				context_->stones[context_->previous_clicked.y()][context_->previous_clicked.x()];
			std::vector< mhe::vector2<int> > del;
			if (check_match3(context_->stones, del))
			{
				std::cout << "delete row\n";
			}
			context_->stones[pos.y()][pos.x()] = prev_stone;
			context_->clicked = context_->previous_clicked;
		}
	}

	boost::shared_ptr<GameContext> context_;
};

#endif

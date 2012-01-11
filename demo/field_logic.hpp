#ifndef __FIELD_LOGIC_HPP__
#define __FIELD_LOGIC_HPP__

#include "field.hpp"

namespace
{

inline bool is_neighbor(const mhe::vector2<int>& pos1, const mhe::vector2<int>& pos2)
{
	cmn::uint dx = abs(pos1.x() - pos2.x());
	cmn::uint dy = abs(pos1.y() - pos2.y());
	if ( ((dx == 1) && !dy) || (!dx && (dy == 1)) )
	return true;
	return false;
}

inline bool check_match_horizontal(const std::vector< std::vector<int> >& stones,
								   std::pair< mhe::vector2<int>, mhe::vector2<int> >& del)
{
	const int match_need = 3;
	for (size_t i = 0; i < stones.size(); ++i)
	{
		int matched = 1;
		int stone = stones[i][0];
		bool test_result = false;
		for (size_t j = 1; j < stones[i].size(); ++j)
		{
			if ( j == (stones[i].size() - 1) ) test_result = true;			
			if (stones[i][j] == stone)
			{
				++matched;
				if (!test_result) continue;
			}
			else
			{
			    stone = stones[i][j];
				test_result = true;								
			}
			if (test_result && (matched >= match_need))
			{
				del.first = mhe::vector2<int>(i, j + 1 - matched);
				del.second = mhe::vector2<int>(i, j);
				return true;
			}
			matched = 1;					
		} 
	}
	return false;
}

inline bool check_match_vertical(const std::vector< std::vector<int> >& /*stones*/, 
								 std::pair< mhe::vector2<int>, mhe::vector2<int> >& /*del*/)	
{
	return false;
}


inline bool check_match(const std::vector< std::vector<int> >& stones, 
						std::pair< mhe::vector2<int>, mhe::vector2<int> >& del)	
{
	if (check_match_horizontal(stones, del)) return true;
	if (check_match_vertical(stones, del)) return true;
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
		DEBUG_LOG("Clicked: " << context_->clicked.x() << " " << context_->clicked.y());
		mhe::v3d pos(context_->geom.ll().x() + context_->clicked.x() * context_->stone_size,
					 context_->geom.ll().y() + context_->clicked.y() * context_->stone_size, 0);
		mhe::v3d prev_pos(context_->geom.ll().x() + context_->previous_clicked.x() * context_->stone_size,
						  context_->geom.ll().y() + context_->previous_clicked.y() * context_->stone_size, 0);		
		if (context_->previous_clicked.x() < 0)	// nothing clicked before - do nothing	
			update_childs(mhe::game::enable_event, &pos);
		else do_logic(pos, prev_pos);
		return true;
	}

	void do_logic(const mhe::v3d& pos, const mhe::v3d& prev_pos)
	{
		const mhe::vector2<int>& clicked = context_->clicked;
		const mhe::vector2<int>& prev_clicked = context_->previous_clicked;
		if (!is_neighbor(clicked, prev_clicked))
		{
			DEBUG_LOG("Not neighbor: " << clicked.x() << " " << clicked.y() <<
					  " " << prev_clicked.x() << " " << prev_clicked.y());
			update_childs(mhe::game::disable_event, &prev_pos);
			update_childs(mhe::game::enable_event, &pos);
			return;
		}
		DEBUG_LOG("Before check match: " << clicked.x() << " " << clicked.y() <<
				  " " << prev_clicked.x() << " " << prev_clicked.y());
		std::pair< mhe::vector2<int>, mhe::vector2<int> > del;	
		int saved_stone = context_->stones[clicked.y()][clicked.x()];
		context_->stones[clicked.y()][clicked.x()] = context_->stones[prev_clicked.y()][prev_clicked.x()];		
		context_->stones[prev_clicked.y()][prev_clicked.x()] = saved_stone;
		if (check_match(context_->stones, del)) 
		{
			DEBUG_LOG("delete row:" << del.first.x() << " " << del.first.y() << " " <<
				del.second.x() << " " << del.second.y());
			update_childs(mhe::game::disable_event, &prev_pos);
			context_->stone_aspects[stone_index(clicked.x(), clicked.y())].lock()->move(prev_pos - pos);	
			context_->stone_aspects[stone_index(prev_clicked.x(), prev_clicked.y())].lock()->move(pos - prev_pos);
		}
		else
		{
			context_->stones[prev_clicked.y()][prev_clicked.x()] = context_->stones[clicked.y()][clicked.x()];
			context_->stones[clicked.y()][clicked.x()] = saved_stone;
		}			
	}

	boost::shared_ptr<GameContext> context_;
};

#endif

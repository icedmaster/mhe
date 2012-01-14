#ifndef __FIELD_LOGIC_HPP__
#define __FIELD_LOGIC_HPP__

#include "mhe.hpp"

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
				del.first = mhe::vector2<int>(j + 1 - matched, i);
				del.second = mhe::vector2<int>(j, i);
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

#endif

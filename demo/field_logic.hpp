#ifndef __FIELD_LOGIC_HPP__
#define __FIELD_LOGIC_HPP__

#include "mhe.hpp"

const int invalid_pos = -1;
const int blocked_pos = 9;

inline bool is_neighbor(const mhe::vector2<int>& pos1, const mhe::vector2<int>& pos2)
{
	cmn::uint dx = abs(pos1.x() - pos2.x());
	cmn::uint dy = abs(pos1.y() - pos2.y());
	if ( ((dx == 1) && !dy) || (!dx && (dy == 1)) )
	return true;
	return false;
}

inline bool check_match_horizontal(const std::vector< std::vector<int> >& stones,
								   std::vector< mhe::vector2<int> >& del)
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
				for (size_t index = j + 1 - matched; index <= j; ++index)
					del.push_back(mhe::vector2<int>(index, i));
				return true;
			}
			matched = 1;					
		} 
	}
	return false;
}

inline bool check_match_vertical(const std::vector< std::vector<int> >& stones, 
								 std::vector< mhe::vector2<int> >& del)	
{
	const int match_need = 3;
	const size_t columns_number = stones[0].size();
	const size_t rows_number = stones.size();

	for (size_t i = 0; i < columns_number; ++i)
	{
		int matched = 1;
		int stone = stones[0][i];
		bool test_result = false;
		for (size_t j = 1; j < rows_number; ++j)
		{
			if ( j == (rows_number - 1) ) test_result = true;			
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
				for (size_t index = j + 1 - matched; index <= j; ++index)
					del.push_back(mhe::vector2<int>(i, index));
				return true;
			}
			matched = 1;
		}	
	}
	return false;
}


inline bool check_match(const std::vector< std::vector<int> >& stones, 
						std::vector< mhe::vector2<int> >& del)	
{
	if (check_match_horizontal(stones, del)) return true;
	if (check_match_vertical(stones, del)) return true;
	return false;
}

inline int get_unblocked_in_vertical_up(const std::vector< std::vector<int> >& stones,
										int column, int pos)
{
	for (size_t i = static_cast<size_t>(pos) + 1; i < stones.size(); ++i)
	{
		if (stones[i][column] != blocked_pos) return i;
	}
	return invalid_pos;
}

inline int get_unblocked_in_vertical_down(const std::vector< std::vector<int> >& stones,
										  int column, int pos)
{
	if (!pos) return invalid_pos;
	for (size_t i = static_cast<size_t>(pos) - 1; i >= stones.size(); --i)
	{
		if (stones[i][column] != blocked_pos) return i;
	}
	return invalid_pos;
}

#endif

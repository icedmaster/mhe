#ifndef __FIELD_LOGIC_HPP__
#define __FIELD_LOGIC_HPP__

#include "mhe.hpp"
#include "game_context.hpp"

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
								   std::vector< std::vector< mhe::vector2<int> > >& del)
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
			if ( (stones[i][j] == stone) && (stones[i][j] != blocked_pos) )
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
				std::vector< mhe::vector2<int> > tmp;
				tmp.reserve(matched);
				for (size_t index = j + 1 - matched; index <= j; ++index)
					tmp.push_back(mhe::vector2<int>(index, i));
				del.push_back(tmp);
				break;
			}
			matched = 1;					
		} 
	}
	return !del.empty();
}

inline bool check_match_vertical(const std::vector< std::vector<int> >& stones, 
								 std::vector< std::vector< mhe::vector2<int> > >& del)	
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
			if ( (stones[j][i] == stone) && (stones[j][i] != blocked_pos) )
			{
				++matched;
				if (!test_result) continue;
			}
			else
			{
			    stone = stones[j][i];
				test_result = true;								
			}
			if (test_result && (matched >= match_need))
			{
				std::vector< mhe::vector2<int> > tmp;
				tmp.reserve(matched);
				for (size_t index = j + 1 - matched; index <= j; ++index)
					tmp.push_back(mhe::vector2<int>(i, index));
				del.push_back(tmp);
				break;
			}
			matched = 1;
		}	
	}
	return !del.empty();
}


inline bool check_match(const std::vector< std::vector<int> >& stones, 
						std::vector< std::vector< mhe::vector2<int> > >& del)	
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
	for (size_t i = pos; i > 0; --i)
	{
		if (stones[i - 1][column] != blocked_pos) return i - 1;
	}
	return invalid_pos;
}

inline void print_field(const std::vector< std::vector<int> >& stones)
{
	DEBUG_LOG("Print field:\t");
	std::string value;
	for (size_t i = 0; i < stones.size(); ++i)
	{
		for (size_t j = 0; j < stones[i].size(); ++j)
		{
			value += (mhe::utils::to_string(stones[i][j]) + " ");
		}
		value += "\n\t";
	}
	DEBUG_LOG(value);
}

#endif

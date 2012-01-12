#include "field.hpp"
#include "field_logic.hpp"

void GameField::init_field(mhe::game::mhe_loader& loader)
{
	context_.stone_size = 50;
	int stone_size = context_.stone_size;
	for (size_t i = 0; i < stones_.size(); ++i)
	{
		for (size_t j = 0; j < stones_[i].size(); ++j)
		{
			StoneParameters sp;
			sp.name = "sprite" + mhe::utils::to_string(i) + mhe::utils::to_string(j);
			sp.sprite_name = get_sprite_name(stones_[i][j]);
			sp.pos = mhe::rect<int>(coord_.ll().x() + j * stone_size, coord_.ll().y() + i * stone_size,
									stone_size, stone_size);
			context_.stone_aspects[stone_index(j, i)] = create_stone(loader, sp);
		}
	}
}

std::string GameField::get_sprite_name(int value) const
{
	switch (value)
	{
	case 0: return "test_stone";
	case 1: return "test_stone2";
	default: return "";
	}
}

bool GameField::on_mouse_move(const mhe::Event& e)
{
	const mhe::MouseEvent& me = dynamic_cast<const mhe::MouseEvent&>(e);
	if (!mouse_on_field(me)) return true;
	return true;
}

bool GameField::on_mouse_click(const mhe::Event& e)
{
	const mhe::MouseEvent& me = dynamic_cast<const mhe::MouseEvent&>(e);
	if (!mouse_on_field(me)) return true;
	const mhe::vector2<int>& pos = get_stone_by_mouse_position(me);
	DEBUG_LOG("Mouse click: " << pos.x() << " " << pos.y());
	return true;
}

bool GameField::mouse_on_field(const mhe::MouseEvent& me) const
{
	return coord_.in(me.x(), me.y());
}

mhe::vector2<int> GameField::get_stone_by_mouse_position(const mhe::MouseEvent& me) const
{
	int x = (me.x() - coord_.ll().x()) / context_.stone_size;
	int y = (me.y() - coord_.ll().y()) / context_.stone_size;
	return mhe::vector2<int>(x, y);
}


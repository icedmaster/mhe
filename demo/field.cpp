#include "field.hpp"
#include "field_logic.hpp"

void GameField::init_field(mhe::game::mhe_loader& loader)
{
	// first, init game context
	context_.stone_size = 50;
	context_.selected.set(-1, -1);
	context_.prev_selected.set(-1, -1);
	context_.clicked.set(-1, -1);
	context_.prev_clicked.set(-1, -1);
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
	if (!mouse_on_field(me))
	{
		if (context_.prev_selected.x() >= 0)
		{
			engine_->get_game_scene()->getScene()->remove(move_effect_);
		    context_.prev_selected.set(-1, -1);
		}
		return true;
	}
	const mhe::vector2<int>& pos = get_stone_by_mouse_position(me);
	handle_move(pos);
	return true;
}

bool GameField::on_mouse_click(const mhe::Event& e)
{
	const mhe::MouseEvent& me = dynamic_cast<const mhe::MouseEvent&>(e);
	if (!mouse_on_field(me)) return true;
	const mhe::vector2<int>& pos = get_stone_by_mouse_position(me);
	handle_click(pos);
	return true;
}

bool GameField::mouse_on_field(const mhe::MouseEvent& me) const
{
	return mhe::rect<int>(coord_.ll().x() + 1, coord_.ll().y() + 1,
						  coord_.width() - 1, coord_.height() - 1).
						 in(me.x(), me.y());
}

mhe::vector2<int> GameField::get_stone_by_mouse_position(const mhe::MouseEvent& me) const
{
	int x = (me.x() - coord_.ll().x()) / context_.stone_size;
	int y = (me.y() - coord_.ll().y()) / context_.stone_size;
	return mhe::vector2<int>(x, y);
}

void GameField::handle_move(const mhe::vector2<int>& pos)
{
	if (pos != context_.prev_selected)
	{
		translate_effect(move_effect_, pos);
		if (context_.prev_selected.x() < 0)
		{		
			engine_->get_game_scene()->getScene()->add(move_effect_);
			move_effect_->start();			
		}
		context_.prev_selected = pos;
	}
}

void GameField::handle_click(const mhe::vector2<int>& pos)
{
	if (pos != context_.prev_clicked)
	{
		translate_effect(select_effect_, pos, mhe::v3d(context_.stone_size / 2, context_.stone_size / 2, 0));
		if (context_.prev_clicked.x() < 0)
		{
			engine_->get_game_scene()->getScene()->add(select_effect_);
			select_effect_->start();
		}
		else do_logic(pos);
		context_.prev_clicked = pos;
	}
}

void GameField::translate_effect(boost::shared_ptr<mhe::iNode> effect, const mhe::vector2<int>& pos,
								 const mhe::v3d& correction)
{
	effect->identity();
	effect->translate(get_global_pos(pos) + correction);
}

mhe::v3d GameField::get_global_pos(const mhe::vector2<int>& pos) const
{
	return mhe::v3d(pos.x() * context_.stone_size + coord_.ll().x(),
					pos.y() * context_.stone_size + coord_.ll().y(), 
					0);
}

void GameField::do_logic(const mhe::vector2<int>& pos)
{
	if (!is_neighbor(context_.prev_clicked, pos)) return;
	std::pair< mhe::vector2<int>, mhe::vector2<int> > del;	
	// before check
	int saved_stone = stones_[pos.y()][pos.x()];
	stones_[pos.y()][pos.x()] = stones_[context_.prev_clicked.y()][context_.prev_clicked.x()];
	stones_[context_.prev_clicked.y()][context_.prev_clicked.x()] = saved_stone;
	if (check_match(stones_, del))
	{
		DEBUG_LOG("Delete row: " << del.first.x() << " " << del.first.y() << " " <<
				  del.second.x() << " " << del.second.y());
		// move 2 aspects
		mhe::vector2<int> move_dir = context_.prev_clicked - pos;
		mhe::v3d v(move_dir.x() * context_.stone_size / 5,
				   move_dir.y() * context_.stone_size / 5, 0);
		mhe::game::MoveParams mp;
		mhe::matrixf m;
		m.set_translate(v);
		mp.m = m;
		mp.move_count = 5;
		mp.upd_time = 20;
		context_.stone_aspects[stone_index(pos.x(), pos.y())].lock()->update(mhe::game::transform_event, &mp);
		m.load_identity();
		m.set_translate(-v);
		mp.m = m;
		context_.stone_aspects[stone_index(context_.prev_clicked.x(), context_.prev_clicked.y())].
			lock()->update(mhe::game::transform_event, &mp);
		engine_->get_game_scene()->getScene()->remove(select_effect_);
	}
	else
	{
		stones_[context_.prev_clicked.y()][context_.prev_clicked.x()] = stones_[pos.y()][pos.x()];
		stones_[pos.y()][pos.x()] = saved_stone;
	}
}

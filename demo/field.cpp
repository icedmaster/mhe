#include "field.hpp"

void GameField::init_field(mhe::game::mhe_loader& loader)
{
	// first, init game context
	context_.stone_size = 50;
	context_.selected.set(-1, -1);
	context_.prev_selected.set(-1, -1);
	context_.clicked.set(-1, -1);
	context_.prev_clicked.set(-1, -1);
	context_.aspect_manager = &(engine_->get_aspect_manager());
	context_.upd_time = 20;
	context_.stone_moves = 5;
	int stone_size = context_.stone_size;
	for (size_t i = 0; i < context_.stones.size(); ++i)
	{
		for (size_t j = 0; j < context_.stones[i].size(); ++j)
		{
			StoneParameters sp;
			sp.name = "sprite" + mhe::utils::to_string(i) + mhe::utils::to_string(j);
			sp.sprite_name = get_sprite_name(context_.stones[i][j]);
			sp.pos = mhe::rect<int>(context_.coord.ll().x() + j * stone_size, context_.coord.ll().y() + i * stone_size,
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
	return mhe::rect<int>(context_.coord.ll().x() + 1, context_.coord.ll().y() + 1,
						  context_.coord.width() - 1, context_.coord.height() - 1).
						 in(me.x(), me.y());
}

mhe::vector2<int> GameField::get_stone_by_mouse_position(const mhe::MouseEvent& me) const
{
	return get_local_pos(context_, mhe::v3d(me.x(), me.y(), 0));
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
	effect->translate(get_global_pos(context_, pos) + correction);
}

void GameField::do_logic(const mhe::vector2<int>& pos)
{
	if (!is_neighbor(context_.prev_clicked, pos)) return;
	std::vector< mhe::vector2<int> > del;	
	// before check
	int cur_index = stone_index(pos.x(), pos.y());
	int prev_index = stone_index(context_.prev_clicked.x(), context_.prev_clicked.y());
	// swap aspects
	boost::weak_ptr<mhe::game::Aspect> tmp = context_.stone_aspects[cur_index];
	context_.stone_aspects[cur_index] = context_.stone_aspects[prev_index];
	context_.stone_aspects[prev_index] = tmp;
	// swap stones
	int saved_stone = context_.stones[pos.y()][pos.x()];
	context_.stones[pos.y()][pos.x()] = context_.stones[context_.prev_clicked.y()][context_.prev_clicked.x()];
	context_.stones[context_.prev_clicked.y()][context_.prev_clicked.x()] = saved_stone;
	if (check_match(context_.stones, del))
	{
		DEBUG_LOG("Delete row: " << del.front().x() << " " << del.front().y() << " " <<
				  del.back().x() << " " << del.back().y());
		for (size_t i = 0; i < del.size(); ++i)
			context_.stones[del[i].y()][del[i].x()] = 9;
		// move 2 aspects
		mhe::vector2<int> move_dir = context_.prev_clicked - pos;
		mhe::v3d v(move_dir.x() * context_.stone_size / context_.stone_moves,
				   move_dir.y() * context_.stone_size / context_.stone_moves, 0);
		mhe::game::MoveParams mp;
		mhe::matrixf m;
		m.set_translate(v);
		mp.m = m;
		mp.move_count = context_.stone_moves;
		mp.upd_time = context_.upd_time;
		context_.stone_aspects[prev_index].lock()->update(mhe::game::transform_event, &mp);
		m.load_identity();
		m.set_translate(-v);
		mp.m = m;
		// attach new aspect before moving
		boost::shared_ptr<mhe::game::Aspect> aspect =
			context_.stone_aspects[cur_index].lock();
		const mhe::v3d correction(context_.stone_size / 2, context_.stone_size / 2, 0);
		std::vector<mhe::v3d> row(del.size());
		for (size_t i = 0; i < del.size(); ++i)
			row[i] = mhe::v3d(get_global_pos(context_, del[i]) + correction);
		boost::shared_ptr<StoneRemoveAspect> remove_aspect(
			new StoneRemoveAspect(aspect->name(), "remove", &context_,
								  del,
								  row,
								  effect_factory_, engine_->get_game_scene()->getScene()));
		engine_->get_aspect_manager().add(remove_aspect);
		aspect->attach(remove_aspect);
		aspect->update(mhe::game::transform_event, &mp);
		engine_->get_game_scene()->getScene()->remove(select_effect_);
	}
	else
	{
		context_.stones[context_.prev_clicked.y()][context_.prev_clicked.x()] = context_.stones[pos.y()][pos.x()];
		context_.stones[pos.y()][pos.x()] = saved_stone;
	}
}

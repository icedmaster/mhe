#include "field.hpp"

void GameField::init_field(mhe::game::mhe_loader& loader)
{
	// first, init game context
	context_.stone_size = 96;
	context_.prev_selected.set(-1, -1);
	context_.prev_clicked.set(-1, -1);
	context_.upd_time = 20;
	context_.stone_moves = 5;
	context_.effect_factory = effect_factory_;
	context_.engine = engine_;

	std::map<int, std::string> types;
	types[0] = "test_stone";
	types[1] = "test_stone2";
	types[2] = "test_stone3";
	types[3] = "test_stone4";
	context_.stone_factory.reset(new StoneFactory(loader, types));
	for (size_t i = 0; i < context_.stones.size(); ++i)
	{
		for (size_t j = 0; j < context_.stones[i].size(); ++j)
		{
			const mhe::v3d& pos = get_global_pos(context_, mhe::vector2<int>(j, i));
			context_.stone_aspects[stone_index(j, i)] =
				context_.stone_factory->create_stone(context_.stones[i][j], pos);
		}
	}
}

void GameField::reload_field()
{
	INFO_LOG("reload_field() called");
	for (size_t i = 0; i < context_.stones.size(); ++i)
	{
		for (size_t j = 0; j < context_.stones[i].size(); ++j)
		{
			const mhe::v3d& pos = get_global_pos(context_, mhe::vector2<int>(j, i));
			int type = context_.stone_factory->get_random_type();
			context_.stone_aspects[stone_index(j, i)] =
				context_.stone_factory->create_stone(type, pos);
			context_.stones[i][j] = type;
		}
	}
	remove_stone_logic(context_);
}

std::string GameField::get_sprite_name(int value) const
{
	switch (value)
	{
	case 0: return "test_stone";
	case 1: return "test_stone2";
		case 2: return "test_stone3";
		case 3: return "test_stone4";
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
	DEBUG_LOG("GameField::handle_click pos:" << pos.x() << " " << pos.y() <<
			  " prev_pos:" << context_.prev_clicked.x() << " " << context_.prev_clicked.y());
	if (pos != context_.prev_clicked)
	{
		bool update_context = true;
		translate_effect(select_effect_, pos, mhe::v3d(context_.stone_size / 2, context_.stone_size / 2, 0));
		if (context_.prev_clicked.x() < 0)
		{
			engine_->get_game_scene()->getScene()->add(select_effect_);
			select_effect_->start();
		}
		else 
		{
			if (do_logic(pos))
			{
				engine_->get_game_scene()->getScene()->remove(select_effect_);
				context_.prev_clicked.set(-1, -1);
				update_context = false;
			}
		}
		if (update_context) context_.prev_clicked = pos;
	}
}

void GameField::translate_effect(boost::shared_ptr<mhe::iNode> effect, const mhe::vector2<int>& pos,
								 const mhe::v3d& correction)
{
	effect->identity();
	effect->translate(get_global_pos(context_, pos) + correction);
}

bool GameField::do_logic(const mhe::vector2<int>& pos)
{
	if (!is_neighbor(context_.prev_clicked, pos))
	{
		return false;
	}
	boost::shared_ptr<mhe::game::Aspect> aspect = context_.stone_aspects[stone_index(pos.x(), pos.y())].lock();
	boost::shared_ptr<RemoveStoneLogicAspect> remove_logic_aspect(
		new RemoveStoneLogicAspect(aspect->name(), "remove_logic", &context_));
	aspect->attach(remove_logic_aspect, mhe::game::end_event);
	context_.engine->get_aspect_manager().add(remove_logic_aspect);
	boost::shared_ptr<MoveStoneAspect> move_aspect(
		new MoveStoneAspect(aspect->name(), "moveback", &context_, context_.prev_clicked, pos));
	remove_logic_aspect->attach(move_aspect, mhe::game::error_event);
	context_.engine->get_aspect_manager().add(move_aspect);
	swap_stones(context_, context_.prev_clicked, pos);

	return true;
}

void swap_stones(GameContext& context, const mhe::vector2<int>& prev, const mhe::vector2<int>& cur)
{
	DEBUG_LOG("swap_stones:" << prev.x() << " " << prev.y() << " " << cur.x() << " " << cur.y());
	int prev_index = stone_index(prev.x(), prev.y());
	int cur_index = stone_index(cur.x(), cur.y());
	// swap aspects
	boost::weak_ptr<mhe::game::Aspect> tmp = context.stone_aspects[prev_index];
	context.stone_aspects[prev_index] = context.stone_aspects[cur_index];
	context.stone_aspects[cur_index] = tmp;
	// swap stones
	int saved = context.stones[prev.y()][prev.x()];
	context.stones[prev.y()][prev.x()] = context.stones[cur.y()][cur.x()];
	context.stones[cur.y()][cur.x()] = saved;
	// calculate direction
	const mhe::vector2<int>& dir = prev - cur;
	mhe::v3d v(dir.x() * context.stone_size / static_cast<int>(context.stone_moves),
			   dir.y() * context.stone_size / static_cast<int>(context.stone_moves), 0);
	DEBUG_LOG("swap_stones: dir:" << dir.x() << " " << dir.y() << " v:" << v.x() << " " << v.y());
	move_stone(context, prev_index, v);
	move_stone(context, cur_index, -v);
}

void move_stone(GameContext& context, int index, const mhe::v3d& direction)
{
	mhe::game::MoveParams mp;
	mhe::matrixf m;
	m.set_translate(direction);
	mp.m = m;
	mp.move_count = context.stone_moves;
	mp.upd_time = context.upd_time;
	context.stone_aspects[index].lock()->update(mhe::game::transform_event, &mp);
}

bool remove_stone_logic(GameContext& context)
{
	print_field(context.stones);
	std::vector< std::vector< mhe::vector2<int> > > del;	
	int count_deleted = 0;
	while (check_match(context.stones, del))
	{
		for (size_t d = 0; d < del.size(); ++d)
		{
			DEBUG_LOG("Delete row: " << del[d].front().x() << " " << del[d].front().y() << " " <<
					  del[d].back().x() << " " << del[d].back().y());
			for (size_t i = 0; i < del[d].size(); ++i)
				context.stones[del[d][i].y()][del[d][i].x()] = blocked_pos;
			// attach new aspect before moving
			boost::shared_ptr<mhe::game::Aspect> aspect =
				context.stone_aspects[stone_index(del[d][0].x(), del[d][0].y())].lock();
			const mhe::v3d correction(context.stone_size / 2, context.stone_size / 2, 0);
			std::vector<mhe::v3d> row(del[d].size());
			for (size_t i = 0; i < del[d].size(); ++i)
				row[i] = mhe::v3d(get_global_pos(context, del[d][i]) + correction);
			remove_stones(context, del[d], row, context.effect_factory);
			context.prev_clicked.set(-1, -1);
			++count_deleted;
		}
		del.clear();
	}
	return count_deleted;
}

void remove_stones(GameContext& context, const std::vector< mhe::vector2<int> >& index_row,
				   const std::vector<mhe::v3d>& row,
				   boost::shared_ptr<StoneEffectFactory> effect_factory)
{
	// create effects for each stone that will be removed
	for (size_t i = 0; i < index_row.size(); ++i)	
	{
		boost::shared_ptr<mhe::iNode> effect = effect_factory->create_remove_stone_effect();
		effect->translate(mhe::v3d(row[i].x(), row[i].y(), 0));
		// get aspect
		boost::shared_ptr<mhe::game::Aspect> aspect =
			context.stone_aspects[stone_index(index_row[i].x(), index_row[i].y())].lock();
		boost::shared_ptr<mhe::game::NodeAspect> effect_aspect(
			new mhe::game::NodeAspect(aspect->name(), "remove_effect", effect, context.engine->get_game_scene()->getScene()));
		if (!i)
		{
			boost::shared_ptr<FieldUpdateAspect> update_aspect(
				new FieldUpdateAspect(aspect->name(), "field_update", &context, index_row));
			context.engine->get_aspect_manager().add(update_aspect);
			aspect->attach(update_aspect);
		}
		context.engine->get_aspect_manager().add(effect_aspect);
		aspect->attach(effect_aspect);
		aspect->set_lifetime(1000);
	}
}

void add_stone(GameContext& context, const mhe::vector2<int>& pos)
{
	const mhe::v3d& gl_pos = get_global_pos(context, pos);
	int type = context.stone_factory->get_random_type();
	DEBUG_LOG("add_stone() create aspect " <<
			  " pos:" << pos.x() << " " << pos.y() << " type:" << type);
	context.stones[pos.y()][pos.x()] = type;
	context.stone_aspects[stone_index(pos.x(), pos.y())] =
		context.stone_factory->create_stone(type, gl_pos);
	context.engine->get_timed_events_manager().add(
		new RemoveStoneLogicEvent(200, &context));
}

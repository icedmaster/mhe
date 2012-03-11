#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include "game_stone.hpp"
#include "field_logic.hpp"
#include "game_context.hpp"
#include "utils/logutils.hpp"

void swap_stones(GameContext& context, const mhe::vector2<int>& prev, const mhe::vector2<int>& cur);
void move_stone(GameContext& context, int index, const mhe::v3d& direction);
bool remove_stone_logic(GameContext& context);
void remove_stones(GameContext& context, const std::vector< mhe::vector2<int> >& index_row,
				   const std::vector<mhe::v3d>& row,
				   boost::shared_ptr<StoneEffectFactory> effect_factory);
void add_stone(GameContext& context, const mhe::vector2<int>& pos);

class RemoveStoneLogicEvent : public mhe::TimedEventBase
{
public:
	RemoveStoneLogicEvent(cmn::uint interval, GameContext* context) :
		mhe::TimedEventBase(interval), 
		context_(context)
	{}

private:
	void execute_impl()
	{
		remove_stone_logic(*context_);
	}
	GameContext* context_;
};

class RemoveStoneLogicAspect : public mhe::game::Aspect
{
public:
	RemoveStoneLogicAspect(const std::string& name, const std::string& add_name,
						   GameContext* context) :
		mhe::game::Aspect(name, add_name), context_(context)
	{}
private:
	void do_subscribe(mhe::game::Aspect*)
	{}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{
		int event_type = remove_stone_logic(*context_) ? mhe::game::success_event : mhe::game::error_event;
		update_children(event_type, nullptr);
		detach_self();
		return true;
	}

	GameContext* context_;
};

class MoveStoneAspect : public mhe::game::Aspect
{
public:
	MoveStoneAspect(const std::string& name, const std::string& add_name,
					GameContext* context,
					const mhe::vector2<int>& prev, const mhe::vector2<int>& cur) :
		mhe::game::Aspect(name, add_name),
		context_(context),
		prev_(prev), cur_(cur)
	{}
private:
	void do_subscribe(mhe::game::Aspect* /*parent*/)
	{}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{
		swap_stones(*context_, prev_, cur_);
		return true;
	}

	GameContext* context_;
	mhe::vector2<int> prev_, cur_;
};

class AddStoneAspect : public mhe::game::Aspect
{
public:
	AddStoneAspect(const std::string& name, const std::string& add_name,
				   GameContext* context, const mhe::vector2<int>& pos) :
		mhe::game::Aspect(name, add_name),
		context_(context), pos_(pos)
	{}
private:
	void do_subscribe(mhe::game::Aspect* /* parent */)
	{}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{
		add_stone(*context_, pos_);				
		return true;
	}

	GameContext* context_;
	mhe::vector2<int> pos_;
};

class FieldUpdateAspect : public mhe::game::Aspect
{
public:
	FieldUpdateAspect(const std::string& name, const std::string& add_name,
					  GameContext* context, const std::vector< mhe::vector2<int> >& indexes) :
		mhe::game::Aspect(name, add_name),
		context_(context), indexes_(indexes)
	{}

private:
	void do_subscribe(mhe::game::Aspect* /*parent*/)
	{}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{
		return true;
	}

	void destroy_impl()
	{
		std::vector<int> columns;
		size_t counter = 0;
		columns.reserve(indexes_.size());
		for (size_t i = 0; i < indexes_.size(); ++i)
		{
			DEBUG_LOG("FieldUpdateAspect: " << indexes_[i].x() << " " << indexes_[i].y());
			int column = indexes_[i].x();
			if (std::find(columns.begin(), columns.end(), column) != columns.end()) continue;
			int up_pos = get_unblocked_in_vertical_up(context_->stones, column, indexes_[i].y());
			int down_pos = get_unblocked_in_vertical_down(context_->stones, column, indexes_[i].y());
			int moves = up_pos - down_pos - 1;
			DEBUG_LOG("FieldUpdateAspect::destroy up_pos=" << up_pos << " down_pos=" << down_pos);
			if (moves > 0)
			{
				mhe::v3d move_dir(0, -moves * context_->stone_size / (int)context_->stone_moves, 0);
				mhe::game::MoveParams mp;
				mp.m.set_translate(move_dir);
				mp.move_count = context_->stone_moves;
				mp.upd_time = context_->upd_time;
				for (size_t y = up_pos; y < context_->stones.size(); ++y)
				{
					if (context_->stone_aspects[stone_index(column, y)].expired()) continue;
					context_->stone_aspects[stone_index(column, y)].lock()->update(mhe::game::transform_event, &mp);
					context_->stones[y - moves][column] = context_->stones[y][column];
					context_->stone_aspects[stone_index(column, y - moves)] = context_->stone_aspects[stone_index(column, y)];
					context_->stone_aspects[stone_index(column, y)].reset();
					boost::shared_ptr<mhe::game::Aspect> parent =
						context_->stone_aspects[stone_index(column, y - moves)].lock();
					boost::shared_ptr<AddStoneAspect> add_aspect(
						new AddStoneAspect(parent->name(), "add_stone", context_,
										   mhe::vector2<int>(column, y)));
					context_->engine->get_aspect_manager().add(add_aspect);
					parent->attach(add_aspect, mhe::game::end_event);
				}
				
				// attach new aspect for creating stones
				for (int j = 0; j < moves; ++j, ++counter)
				{
					boost::shared_ptr<mhe::game::Aspect> parent = 
						get_aspect(*context_,
								   mhe::vector2<int>(column,
													 context_->stones.size() - moves - 1));
					if (parent == nullptr) break;
					const std::string& parent_name = parent->name();
					boost::shared_ptr<AddStoneAspect> add_aspect(
						new AddStoneAspect(parent_name, "add_stone" + mhe::utils::to_string(j), context_,
										   mhe::vector2<int>(column, context_->stones.size() - j - 1)));
					context_->engine->get_aspect_manager().add(add_aspect);
					parent->attach(add_aspect, mhe::game::end_event);
				}				
				columns.push_back(column);
			}
			else
				add_stone(*context_, indexes_[i]);
		}
	}

	GameContext* context_;
	std::vector< mhe::vector2<int> > indexes_;
};

class StoneRemoveAspect : public mhe::game::Aspect
{
public:
	StoneRemoveAspect(const std::string& name, const std::string& add_name,
					  GameContext* game_context,
					  const std::vector< mhe::vector2<int> >& index_row,
					  const std::vector<mhe::v3d>& row,
					  boost::shared_ptr<StoneEffectFactory> effect_factory,
					  boost::shared_ptr<mhe::Scene> scene) :
		mhe::game::Aspect(name, add_name),
		context_(game_context),
		index_row_(index_row), remove_row_(row),
		effect_factory_(effect_factory), scene_(scene)
	{}
private:
	void do_subscribe(mhe::game::Aspect* parent)
	{
		parent->subscribe(mhe::game::end_event, this);
	}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{		
		// create effects for each stone that will be removed
		for (size_t i = 0; i < index_row_.size(); ++i)	
		{
			boost::shared_ptr<mhe::iNode> effect = effect_factory_->create_remove_stone_effect();
			effect->translate(mhe::v3d(remove_row_[i].x(), remove_row_[i].y(), 0));
			// get aspect
			boost::shared_ptr<mhe::game::Aspect> aspect =
				context_->stone_aspects[stone_index(index_row_[i].x(), index_row_[i].y())].lock();
			boost::shared_ptr<mhe::game::NodeAspect> effect_aspect(
				new mhe::game::NodeAspect(aspect->name(), "remove_effect", effect, scene_));
			if (!i)
			{
				boost::shared_ptr<FieldUpdateAspect> update_aspect(
					new FieldUpdateAspect("field", "update", context_, index_row_));
				context_->engine->get_aspect_manager().add(update_aspect);
				aspect->attach(update_aspect);
			}
			context_->engine->get_aspect_manager().add(effect_aspect);
			aspect->attach(effect_aspect);
			aspect->set_lifetime(1000);
		}
		return true;
	}

	GameContext* context_; 
	std::vector< mhe::vector2<int> > index_row_;
	std::vector<mhe::v3d> remove_row_;
	boost::shared_ptr<StoneEffectFactory> effect_factory_;
	boost::shared_ptr<mhe::Scene> scene_; 
};

class GameField
{
private:
	// helper class for handling events
	typedef mhe::PrivateEventListener<GameField> GameFieldEventListener;
	friend class mhe::PrivateEventListener<GameField>;
public:
	GameField(mhe::game::mhe_loader& loader,
			  const mhe::rect<int>& coord,
			  const std::vector< std::vector<int> >& stones,
			  StoneEffectFactory* effect_factory) :
		effect_factory_(effect_factory),
		move_listener_(new GameFieldEventListener(mhe::MouseEventType, mhe::MOUSE_MOVE, 0, this, &GameField::on_mouse_move)),
		click_listener_(new GameFieldEventListener(mhe::MouseEventType, mhe::MOUSE_BUTTON_PRESSED, 0, this, &GameField::on_mouse_click)),
		engine_(loader.get_engine())
	{
		if (effect_factory_ != nullptr)
		{
			move_effect_ = effect_factory_->create_move_stone_effect();
			select_effect_ = effect_factory_->create_select_stone_effect();
			remove_effect_ = effect_factory_->create_remove_stone_effect();
		}
		loader.get_engine()->getInputSystem()->addListener(move_listener_);
		loader.get_engine()->getInputSystem()->addListener(click_listener_);
		context_.coord = coord;
		context_.stones = stones;
		init_field(loader);
	}

	void reload_field();
private:
	void init_field(mhe::game::mhe_loader& loader);
	std::string get_sprite_name(int value) const;
	bool on_mouse_move(const mhe::Event& e);
	bool on_mouse_click(const mhe::Event& e);
	bool mouse_on_field(const mhe::MouseEvent& me) const;
	mhe::vector2<int> get_stone_by_mouse_position(const mhe::MouseEvent& me) const;
	void handle_move(const mhe::vector2<int>& pos);
	void handle_click(const mhe::vector2<int>& pos);
	void translate_effect(boost::shared_ptr<mhe::iNode> effect, const mhe::vector2<int>& pos,
						  const mhe::v3d& correction = mhe::v3d());
	bool do_logic(const mhe::vector2<int>& pos);

	typedef std::vector< std::vector<int> > field_type;
	boost::shared_ptr<StoneEffectFactory> effect_factory_;
	GameContext context_;
	boost::shared_ptr<mhe::iNode> move_effect_;
	boost::shared_ptr<mhe::iNode> select_effect_;
	boost::shared_ptr<mhe::iNode> remove_effect_;

	// events
	boost::shared_ptr<GameFieldEventListener> move_listener_;
	boost::shared_ptr<GameFieldEventListener> click_listener_;
	bool mouse_on_;

	mhe::game::Engine* engine_;
};

#endif

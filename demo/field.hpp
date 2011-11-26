#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include "game_stone.hpp"

class GameField
{
public:
	GameField(const mhe::game::mhe_loader& loader, const mhe::rect<int>& coord, 
			  const std::vector< std::vector<int> >& stones);
private:
	std::string get_sprite_name(int value) const;
};

struct GameContext
{	
	mhe::rect<int> geom;
	int stone_size;
	std::vector< std::vector<int> > stones;
	mhe::vector2<int> previous_selected;
	mhe::vector2<int> selected;
	mhe::vector2<int> clicked;
	mhe::vector2<int> previous_clicked;
	std::map< int, boost::weak_ptr<mhe::game::Aspect> > stone_aspects;
	const void* magic;
};

class FieldGUIAspect : public mhe::game::Aspect
{
public:
	FieldGUIAspect(const std::string& name,
				   boost::shared_ptr<GameContext> context) :
		mhe::game::Aspect(name),
		context_(context)
	{
		context_->selected.set(-1, -1);
		context_->previous_selected.set(-1, -1);
		context_->clicked.set(-1, -1);
		context_->previous_clicked.set(-1, -1);
	}
private:
	void do_subscribe(Aspect* parent)
	{
		parent->subscribe(mhe::game::mouse_on_event, this);
		parent->subscribe(mhe::game::mouse_move_int_event, this);
		parent->subscribe(mhe::game::mouse_left_event, this);
		parent->subscribe(mhe::game::mouse_click_event, this);
	}

	bool update_impl(int type, const void* arg)	
	{		
		const mhe::MouseEvent* me = static_cast<const mhe::MouseEvent*>(arg);
		if ( (type == mhe::game::mouse_on_event) || (type == mhe::game::mouse_move_int_event) )
			process_mouse_on_event(me);
		else if (type == mhe::game::mouse_left_event)
			process_mouse_left_event(me);
		else if (type == mhe::game::mouse_click_event)
			process_mouse_click_event(me);
		return true;
	}

	void process_mouse_on_event(const mhe::MouseEvent* me)
	{		
		const mhe::rect<int>& coord = context_->geom;
		mhe::vector2<int> pos((me->x() - coord.ll().x()) / context_->stone_size,
							  (me->y() - coord.ll().y()) / context_->stone_size);	
		if (context_->selected != pos)
		{
			if (context_->selected.x() < 0)
			{
				// first time
				context_->selected = pos;
				update_childs(mhe::game::mouse_on_event, me);
			}
			else
			{
				context_->previous_selected = context_->selected;
				context_->selected = pos;
				update_childs(mhe::game::mouse_left_event, me);				
				update_childs(mhe::game::mouse_on_event, me);
			}
		}	
	}

	void process_mouse_click_event(const mhe::MouseEvent* me)
	{
		const mhe::rect<int>& coord = context_->geom;
		mhe::vector2<int> pos((me->x() - coord.ll().x()) / context_->stone_size,
							  (me->y() - coord.ll().y()) / context_->stone_size);
		if (context_->clicked != pos)
		{
			context_->previous_clicked = context_->clicked;
			context_->clicked = pos;
			update_childs(mhe::game::mouse_click_event, me);
		}
	}

	void process_mouse_left_event(const mhe::MouseEvent* me)
	{
		if (context_->selected.x() >= 0)
		{
			context_->previous_selected = context_->selected;
			update_childs(mhe::game::mouse_left_event, me);
			context_->previous_selected.set(-1, -1);
			context_->selected.set(-1, -1);
		}
	}

	boost::shared_ptr<GameContext> context_;
};

class FieldMouseMoveAspect : public mhe::game::Aspect
{
public:
	FieldMouseMoveAspect(const std::string& name, boost::shared_ptr<GameContext> context) :
		mhe::game::Aspect(name), context_(context)
	{}
private:
	void do_subscribe(Aspect* parent)
	{
		parent->subscribe(mhe::game::mouse_on_event, this);
		parent->subscribe(mhe::game::mouse_left_event, this);
	}

	bool update_impl(int type, const void* arg)
	{		
		mhe::v3d pos(context_->geom.ll().x() + context_->selected.x() * context_->stone_size,
					 context_->geom.ll().y() + context_->selected.y() * context_->stone_size, 0);
		if (type == mhe::game::mouse_left_event)
			update_childs(mhe::game::mouse_left_event, arg);
		else
			update_childs(type, &pos);		
		return true;
	}

	boost::shared_ptr<GameContext> context_; 
};

#endif

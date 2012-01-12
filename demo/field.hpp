#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include "game_stone.hpp"

inline int stone_index(int x, int y)
{
	return (x << 8) | y;
}

struct GameContext
{	
	int stone_size;
	mhe::vector2<int> selected;
	mhe::vector2<int> prev_selected;
	std::map< int, boost::weak_ptr<mhe::game::Aspect> > stone_aspects;
};

class StoneEffectFactory
{
public:
	virtual boost::shared_ptr<mhe::iNode> create_move_stone_effect() const = 0;
	virtual boost::shared_ptr<mhe::iNode> create_select_stone_effect() const = 0;
	virtual boost::shared_ptr<mhe::iNode> create_remove_stone_effect() const = 0;
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
		coord_(coord), stones_(stones), effect_factory_(effect_factory),
		move_listener_(new GameFieldEventListener(mhe::MouseEventType, mhe::MOUSE_MOVE, 0, this, &GameField::on_mouse_move)),
		click_listener_(new GameFieldEventListener(mhe::MouseEventType, mhe::MOUSE_BUTTON_PRESSED, 0, this, &GameField::on_mouse_click)),
		engine_(loader.get_engine())
	{
		if (effect_factory_ != nullptr)
		{
			move_effect_ = effect_factory_->create_move_stone_effect();
			select_effect_ = effect_factory_->create_select_stone_effect();
		}
		loader.get_engine()->getInputSystem()->addListener(move_listener_);
		loader.get_engine()->getInputSystem()->addListener(click_listener_);
		init_field(loader);
	}
private:
	void init_field(mhe::game::mhe_loader& loader);
	std::string get_sprite_name(int value) const;
	bool on_mouse_move(const mhe::Event& e);
	bool on_mouse_click(const mhe::Event& e);
	bool mouse_on_field(const mhe::MouseEvent& me) const;
	mhe::vector2<int> get_stone_by_mouse_position(const mhe::MouseEvent& me) const;
	void handle_move(const mhe::vector2<int>& pos);
	void translate_move_effect(const mhe::vector2<int>& pos);
	mhe::v3d get_global_pos(const mhe::vector2<int>& pos) const;

	typedef std::vector< std::vector<int> > field_type;
	mhe::rect<int> coord_;
	field_type stones_;
	boost::shared_ptr<StoneEffectFactory> effect_factory_;
	GameContext context_;
	boost::shared_ptr<mhe::iNode> move_effect_;
	boost::shared_ptr<mhe::iNode> select_effect_;

	// events
	boost::shared_ptr<GameFieldEventListener> move_listener_;
	boost::shared_ptr<GameFieldEventListener> click_listener_;
	bool mouse_on_;

	mhe::game::Engine* engine_;
};

#endif

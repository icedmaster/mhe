#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include "game_stone.hpp"
#include "utils/logutils.hpp"

inline int stone_index(int x, int y)
{
	return (x << 8) | y;
}

struct GameContext
{	
	int stone_size;
	mhe::vector2<int> selected;
	mhe::vector2<int> prev_selected;
	mhe::vector2<int> clicked;
	mhe::vector2<int> prev_clicked;
	std::map< int, boost::weak_ptr<mhe::game::Aspect> > stone_aspects;
};

class StoneEffectFactory
{
public:
	virtual ~StoneEffectFactory() {}
	virtual boost::shared_ptr<mhe::iNode> create_move_stone_effect() const = 0;
	virtual boost::shared_ptr<mhe::iNode> create_select_stone_effect() const = 0;
	virtual boost::shared_ptr<mhe::iNode> create_remove_stone_effect() const = 0;
}; 

class StoneRemoveAspect : public mhe::game::Aspect
{
public:
	StoneRemoveAspect(const std::string& name, const std::string& add_name,
					  GameContext* game_context, const std::pair< mhe::v3d, mhe::v3d >& row,
					  boost::shared_ptr<StoneEffectFactory> effect_factory,
					  boost::shared_ptr<mhe::Scene> scene) :
		mhe::game::Aspect(name, add_name),
		context_(game_context),
		remove_row_(row), effect_factory_(effect_factory), scene_(scene)
	{}
private:
	void do_subscribe(mhe::game::Aspect* parent)
	{
		parent->subscribe(mhe::game::end_event, this);
	}

	bool update_impl(int /*type*/, const void* /*arg*/)
	{		
		// create effects for each stone that will be removed
		if (remove_row_.first.y() == remove_row_.second.y())	// hor
		{
			float max_x = remove_row_.second.x() + context_->stone_size;
			DEBUG_LOG("StoneRemoveAspect update_impl():" << remove_row_.first.x() << " " << max_x);
			for (float x = remove_row_.first.x(); x < max_x; x += context_->stone_size)
			{
				boost::shared_ptr<mhe::iNode> effect = effect_factory_->create_remove_stone_effect();
				effect->translate(mhe::v3d(x, remove_row_.first.y(), 0));
				scene_->add(effect);
				effect->start();
			}
		}
		return true;
	}

	GameContext* context_; 
	std::pair< mhe::v3d, mhe::v3d > remove_row_;
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
		coord_(coord), stones_(stones), effect_factory_(effect_factory),
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
	void handle_click(const mhe::vector2<int>& pos);
	void translate_effect(boost::shared_ptr<mhe::iNode> effect, const mhe::vector2<int>& pos,
						  const mhe::v3d& correction = mhe::v3d());
	mhe::v3d get_global_pos(const mhe::vector2<int>& pos) const;
	void do_logic(const mhe::vector2<int>& pos);

	typedef std::vector< std::vector<int> > field_type;
	mhe::rect<int> coord_;
	field_type stones_;
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

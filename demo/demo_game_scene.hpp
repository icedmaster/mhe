#ifndef __DEMO_GAME_SCENE_HPP__
#define __DEMO_GAME_SCENE_HPP__

#include "mhe.hpp"
#include "game_stone.hpp"
#include "field.hpp"
#include "circle_particle_effect.hpp"

class DemoStoneEffectFactory : public StoneEffectFactory
{
public:
	DemoStoneEffectFactory(mhe::game::mhe_loader& loader)
	{
		init_move_effect(loader);
		init_select_effect(loader);
		init_remove_effect(loader);
	}

	boost::shared_ptr<mhe::Node> create_move_stone_effect() const
	{
		return move_effect_;
	}

	boost::shared_ptr<mhe::Node> create_select_stone_effect() const
	{
		return select_effect_;
	}

	boost::shared_ptr<mhe::Node> create_remove_stone_effect() const
	{
		boost::shared_ptr<mhe::CircleParticleEffect> peffect(new mhe::CircleParticleEffect(mhe::cfGreen, 25));
		boost::shared_ptr<mhe::ParticleSystem> ps(new mhe::ParticleSystem(20, mhe::v3d(), true));
		ps->addEffect(peffect);
		ps->setSize(5);
		ps->setPriority(4);
		return ps;
	}
private:
	void init_move_effect(mhe::game::mhe_loader& loader)
	{
		move_effect_.reset(loader.getSprite(L"stone_highlight"));
		move_effect_->setPriority(4);
	}

	void init_select_effect(mhe::game::mhe_loader& /*loader*/)
	{
		boost::shared_ptr<mhe::CircleParticleEffect> peffect(new mhe::CircleParticleEffect(mhe::cfYellow, 25));
		boost::shared_ptr<mhe::ParticleSystem> ps(new mhe::ParticleSystem(20, mhe::v3d(), true));
		ps->addEffect(peffect);
		ps->setSize(5);
		ps->setPriority(4);
		select_effect_ = ps;
	}

	void init_remove_effect(mhe::game::mhe_loader& /*loader*/)
	{
	}

	boost::shared_ptr<mhe::Node> move_effect_;
	boost::shared_ptr<mhe::Node> select_effect_;
};

class DemoGameScene : public mhe::game::GameScene
{
	typedef mhe::PrivateEventListener<DemoGameScene> DemoGameSceneListener;
	friend class mhe::PrivateEventListener<DemoGameScene>; 
public:
	DemoGameScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		reset_listener_(new DemoGameSceneListener(mhe::KeyboardEventType, mhe::KEY_DOWN, SDLK_r,
												  this, &DemoGameScene::reset_field))
	{
		get_engine()->getInputSystem()->add_listener(reset_listener_);
	}
private:
	bool init_impl(const std::string& arg, void*)
	{
		load_scene(arg);
		return true;
	}

	void load_scene(const std::string& arg)
	{
		mhe::game::mhe_loader loader(arg, get_engine());
		loader.load_all_assets();
		fps_font = get_engine()->getFontManager().get("145");
		fps_font->setColor(mhe::cfWhite);
		boost::shared_ptr<mhe::Node> fps_node(
			new mhe::utils::GraphicsFPSCounter(get_engine()->getInputSystem(),
											   fps_font, mhe::v2d(20, 550)));
		get_scene()->add(fps_node);
		create_field(loader, arg);
		get_engine()->getInputSystem()->enable_input();
	}

	void create_field(mhe::game::mhe_loader& loader, const std::string& arg)
	{
		std::vector<int> row(3);
		std::vector< std::vector<int> > stones;
		row[0] = 0; row[1] = 1; row[2] = 1;
		stones.push_back(row);
		row[0] = 1; row[1] = 0; row[2] = 0;
		stones.push_back(row);
		row[0] = 1; row[1] = 1; row[2] = 0;
		stones.push_back(row);
		game_field.reset(new GameField(loader, mhe::rect<int>(200, 200, 96 * 3, 96 * 3), stones, 
									   new DemoStoneEffectFactory(loader)));
	}
private:
	bool reset_field(const mhe::Event&)
	{
		game_field->reload_field();
		return true;
	}

	boost::shared_ptr<mhe::gui::iFont> fps_font;	
	boost::shared_ptr<GameField> game_field;
	boost::shared_ptr<DemoGameSceneListener> reset_listener_;
};

#endif

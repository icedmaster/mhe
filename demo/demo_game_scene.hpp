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

	boost::shared_ptr<mhe::iNode> create_move_stone_effect() const
	{
		return move_effect_;
	}

	boost::shared_ptr<mhe::iNode> create_select_stone_effect() const
	{
		return select_effect_;
	}

	boost::shared_ptr<mhe::iNode> create_remove_stone_effect() const
	{
		return remove_effect_;
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
		boost::shared_ptr<mhe::CircleParticleEffect> peffect(new mhe::CircleParticleEffect(mhe::cfWhite, 25));
		boost::shared_ptr<mhe::ParticleSystem> ps(new mhe::ParticleSystem(20, mhe::v3d(), true));
		ps->addEffect(peffect);
		ps->setSize(5);
		ps->setPriority(4);
		remove_effect_ = ps;
	}

	boost::shared_ptr<mhe::iNode> move_effect_;
	boost::shared_ptr<mhe::iNode> select_effect_;
	boost::shared_ptr<mhe::iNode> remove_effect_;
};

class DemoGameScene : public mhe::game::GameScene
{
public:
	DemoGameScene(mhe::game::Engine* engine) : mhe::game::GameScene(engine) {}
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
		boost::shared_ptr<mhe::iNode> fps_node(
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
		row[0] = 1; row[1] = 0; row[2] = 1;
		stones.push_back(row);
		game_field.reset(new GameField(loader, mhe::rect<int>(200, 200, 150, 100), stones, 
									   new DemoStoneEffectFactory(loader)));
	}
private:
	boost::shared_ptr<mhe::gui::iFont> fps_font;	
	boost::shared_ptr<GameField> game_field;
};

#endif

#include "field.hpp"
#include "field_logic.hpp"
#include "circle_particle_effect.hpp"
#include "test_particle_effect.hpp"

GameField::GameField(const mhe::game::mhe_loader& loader, const mhe::rect<int>& coord, 
					 const std::vector< std::vector<int> >& stones)
{
	const int size = 50;	
	boost::shared_ptr<mhe::CircleParticleEffect> peffect(new mhe::CircleParticleEffect(mhe::cfYellow, 25));
	boost::shared_ptr<mhe::ParticleSystem> ps(new mhe::ParticleSystem(20, mhe::v3d(), true));
	ps->addEffect(peffect);
	ps->setSize(5);
	ps->setPriority(4);
	// click
	boost::shared_ptr<mhe::Sprite> hl_sprite(loader.getSprite(L"stone_highlight"));
	hl_sprite->setSize(size);	
	hl_sprite->setPriority(4);

	mhe::game::Engine* engine = const_cast<mhe::game::Engine*>(loader.get_engine());

	boost::shared_ptr<GameContext> context(new GameContext);
	context->stone_size = size;
	context->stones = stones;	
	mhe::rect<int> input_coord(coord.ll().x() + 1, coord.ll().y() + 1, 
							   coord.width() - 1, coord.height() - 1);
	context->geom = input_coord;
	boost::shared_ptr<mhe::game::MouseInputAspect> mouse_aspect(
		new mhe::game::MouseInputAspect("field", engine->getInputSystem(), input_coord));
	boost::shared_ptr<FieldGUIAspect> gui_aspect(
		new FieldGUIAspect("field.gui", context));
	mouse_aspect->attach(gui_aspect);
	boost::shared_ptr<FieldMouseMoveAspect> move_aspect(
		new FieldMouseMoveAspect("field.move", context));
	gui_aspect->attach(move_aspect);

	engine->get_aspect_manager().add(mouse_aspect);
	engine->get_aspect_manager().add(gui_aspect);	
	engine->get_aspect_manager().add(move_aspect);

	boost::shared_ptr<StoneAffectorAspect> effect(new StoneAffectorAspect("effect",
																	  	  ps,
																	  	  engine->get_game_scene()->getScene(),
																	  	  mhe::game::enable_event,
																	  	  mhe::game::disable_event,
																		  mhe::v3d(25, 25, 0)));	
	boost::shared_ptr<StoneAffectorAspect> effect1(new StoneAffectorAspect("effect1",
																		   hl_sprite,	
																	   	   engine->get_game_scene()->getScene(),
																	   	   mhe::game::mouse_on_event,
																	   	   mhe::game::mouse_left_event
																		   ));

	engine->get_aspect_manager().add(effect);
	engine->get_aspect_manager().add(effect1);

	boost::shared_ptr<FieldLogicAspect> field_logic_aspect(
		new FieldLogicAspect("field.logic", context));
	engine->get_aspect_manager().add(field_logic_aspect);
	field_logic_aspect->attach(effect);
	gui_aspect->attach(field_logic_aspect);

	move_aspect->attach(effect1);

	for (size_t i = 0; i < stones.size(); ++i)
	{
		for (size_t j = 0; j < stones[i].size(); ++j)
		{
			StoneParameters sp;
			sp.name = "stone" + mhe::utils::to_string(i + j);
			sp.sprite_name = get_sprite_name(stones[i][j]);
			sp.pos = mhe::rect<int>(coord.ll().x() + j * size, coord.ll().y() + i * size, 
									size, size);
			boost::shared_ptr<mhe::game::Aspect> aspect = create_stone(loader, sp);
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


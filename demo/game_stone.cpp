#include "game_stone.hpp"
#include "circle_particle_effect.hpp"
#include "test_particle_effect.hpp"

mhe::game::aspect_ptr create_stone(const mhe::game::mhe_loader& loader,
	 			  				   const StoneParameters& sp)
{
	mhe::game::Engine* engine = const_cast<mhe::game::Engine*>(loader.get_engine());
	
	boost::shared_ptr<mhe::iNode> sprite = engine->get_game_scene()->getScene()->get_node(sp.sprite_name);
	if (sprite == nullptr)
	{
		boost::shared_ptr<mhe::Sprite> loaded_sprite(loader.getSprite(mhe::utils::to_wstring(sp.sprite_name)));
		if (loaded_sprite != nullptr)
			engine->get_game_scene()->getScene()->add(loaded_sprite);
		loaded_sprite->translate(mhe::v3d(sp.pos.ll().x(), sp.pos.ll().y(), 0));
		loaded_sprite->setSize(50);	
		sprite = loaded_sprite;
	}

	boost::shared_ptr<mhe::game::GraphicsAspect> gr(
		new mhe::game::GraphicsAspect(sp.name, sprite));

	engine->get_aspect_manager().add(gr);
	 
	return gr;
}



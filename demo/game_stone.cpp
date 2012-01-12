#include "game_stone.hpp"
#include "game/node_aspect.hpp"

boost::shared_ptr<mhe::game::Aspect> create_stone(mhe::game::mhe_loader& loader,
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

	boost::shared_ptr<mhe::game::NodeAspect> node(new mhe::game::NodeAspect(sp.name, "node", sprite));

	engine->get_aspect_manager().add(node);
	 
	return node;
}


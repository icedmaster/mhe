#include "game_stone.hpp"
#include "game/node_aspect.hpp"
#include "utils/logutils.hpp"

boost::shared_ptr<mhe::game::Aspect> create_stone(mhe::game::mhe_loader& loader,
	 			  				   				  const StoneParameters& sp)
{
	mhe::game::Engine* engine = const_cast<mhe::game::Engine*>(loader.get_engine());
	
	boost::shared_ptr<mhe::iNode> sprite;
	boost::shared_ptr<mhe::iNode> tmp_sprite = engine->get_game_scene()->getScene()->get_node(sp.sprite_name);
	if (sprite == nullptr)
	{
		boost::shared_ptr<mhe::Sprite> loaded_sprite(loader.getSprite(mhe::utils::to_wstring(sp.sprite_name)));
		sprite = loaded_sprite;
	}
	else sprite.reset(tmp_sprite->clone());

	if (sprite == nullptr)
	{
		return boost::shared_ptr<mhe::game::Aspect>();
	}
	sprite->translate(mhe::v3d(sp.pos.ll().x(), sp.pos.ll().y(), 0));
	//sprite->setSize(50);	

	boost::shared_ptr<mhe::game::NodeAspect> node(new mhe::game::NodeAspect(sp.name, "node", sprite, engine->get_game_scene()->getScene()));

	engine->get_aspect_manager().add(node);
	 
	return node;
}

